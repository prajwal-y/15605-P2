/** @file thread.c
 *  @brief Implementation of thread library functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <mutex.h>
#include <asm.h>
#include <list.h>
#include <syscall.h>
#include <thread.h>
#include <tcb.h>
#include <thread.h>
#include <errors.h>
#include <malloc.h>
#include <cond.h>
#include <autostack.h>
#include <contracts.h>
#include <thr_internals.h>

#define TRUE 1
#define FALSE 0


#define STACK_PADDING(size) ((((size)%4)==0)?0:(4-((size)%4)))

static unsigned int stack_size;
static tcb_t head;

static mutex_t tcb_lock;

/*Helper functions*/
static tcb_t *find_tcb(int tid);
static void remove_tcb(tcb_t *tcb);
static void add_tcb(int tid, tcb_t *tcb);
static tcb_t *init_tcb(void *stack_base);

/** @brief This function is responsible for initializing the
 *  thread library.
 *
 *  In a multi threaded environment we do not support growing the stack.
 *  So we first uninstall the software exception handler which grows the stack
 *  in a single threaded environment. This function can be called only by one
 *  thread at a time and only once per process.
 *
 *  @param size Size of the stack space available for each thread
 * 
 *  @return int 0 if initialization is successful. negative number on failure
 */
int thr_init(unsigned int size) {
    int ret_val;

    uninstall_seh();
    stack_size = size + STACK_PADDING(size);
	ret_val = mutex_init(&tcb_lock);
    if (ret_val < 0) {
        return ret_val;
    }
    init_head(&head.tcb_list);
    tcb_t *tcb;
    if ((tcb = init_tcb(NULL)) == NULL) {
        return ERR_INVAL;
    }

	/*Add the current thread to the TCB list*/
	add_tcb(thr_getid(), tcb);

	return 0;
}

/** @brief This function is responsible for creating a thread to 
 *  run the function func(arg).
 *
 *  @param func Function pointer to the function that the thread
 *  needs to run
 *  @param arg Parameters to the function to be called by the thread
 *
 *  @return int If thread creation is successful, the thread ID of the 
 *  new thread is returned. Otherwise, a negative value is returned.
 */
int thr_create(void *(*func)(void *), void *arg) {
    char *stack_base = ((char *)malloc(stack_size));
    if (stack_base == NULL) {
        return ERR_NOMEM;
    }
    tcb_t *tcb;
    if ((tcb = init_tcb(NULL)) == NULL) {
        return ERR_INVAL;
    }
	mutex_lock(&tcb_lock); /*Lock the TCB list for adding a TCB entry*/
	int tid = thread_fork((stack_base + stack_size), func, arg);
	add_tcb(tid, tcb);
	mutex_unlock(&tcb_lock);

	return tid;
}

/** @brief This function is responsible for "cleaning up" a thread with
 *  given tid, optionally returning the status information provided by 
 *  the thread at the time of exit. If the thread is not exited, it
 *  will be suspended until the thread is exited.
 *
 *  @param tid Thread ID of the thread to be cleaned up.
 *  @param statusp The value passed to thr_exit() by the joined thread 
 *  will be placed in the location referenced by statusp.
 *
 *  @return int 0 on success, error code (negative number) on error 
 */
int thr_join(int tid, void **statusp) {
	if (tid < 0) {
		return ERR_INVAL;
	}
	mutex_lock(&tcb_lock);
	tcb_t *tcb = find_tcb(tid);
	mutex_unlock(&tcb_lock);
    if (tcb == NULL) {
        return ERR_INVAL;
    }
    
	mutex_lock(&tcb->tcb_mutex);
	while(tcb->exited != TRUE) {
		cond_wait(&tcb->waiting_threads, &tcb->tcb_mutex);
	}
	if (statusp != NULL) {
    	*statusp = tcb->status;
    }
	
	mutex_lock(&tcb_lock);
	remove_tcb(tcb);
	mutex_unlock(&tcb_lock);
	
	return 0;
}

/** @brief This function exits the thread with exit status.
 *
 *  If the thread does not exist we simply return.
 *  @param status The exit status of the thread.
 *
 *  @return Void 
 */
void thr_exit(void *status) {
	int tid = thr_getid();

	mutex_lock(&tcb_lock);
	tcb_t *tcb = find_tcb(tid);
	mutex_unlock(&tcb_lock);

    if (tcb == NULL) { /* Can happen only if called without calling thr_init */
        vanish();
    }

	mutex_lock(&tcb->tcb_mutex);
	tcb->exited = TRUE;
	tcb->status = status;
	cond_signal(&tcb->waiting_threads);
	mutex_unlock(&tcb->tcb_mutex);
	if(tcb->stack_base != NULL) {
		free(tcb->stack_base);
	}
	vanish();
}

/** @brief wrapper function to install exception handler for new thread
 *        and call thread function
 *
 *  @param func_addr address of thread function
 *  @param arg arguments to thread function
 *  @return Void
 */
void new_thread_init(void *(*func_addr)(void *), void *arg) {	
    install_seh_multi();
    thr_exit(func_addr(arg));	/* in case thr_exit not called by programmer */
}

/** @brief This function returns the thread ID of the current thread
 *
 *  @return int The thread ID of the current thread.
 */
int thr_getid() {
	return gettid();
}

/** @brief Defers execution of the invoking thread to a later time in
 *         favor of the thread with ID.
 *
 *  @param tid Thread ID of the thread whose execution needs to be deferred
 *
 *  @return int 0 if successful, 
 */
int thr_yield(int tid) {
	return yield(tid);
}

/** @brief Function to linearly scan the list of TCBs for the TCB with the
 *         given ID.
 *
 * @param tid Thread ID
 *
 * @return tcb_t The TCB for the given thread ID
 */
tcb_t *find_tcb(int tid) {
	list_head *p = get_first(&head.tcb_list);
	while(p != NULL && p != &head.tcb_list) {
		tcb_t *t = get_entry(p, tcb_t, tcb_list);
		if(t->id == tid) {
			return t;
		}
		p = p->next;
	}
	return NULL;
}

/** @brief Function to initialize a tcb struct
 *
 *  Calls to this function are not thread safe and must be protected
 *  by a mutex.
 *
 *  @param stack_base Base address of the stack of the thread.
 *
 *  @return tcb_t * return pointer to an initialized tcb or NULL if it fails
 */
tcb_t *init_tcb(void *stack_base) {
	tcb_t *tcb = (tcb_t *)malloc(sizeof(tcb_t));
    if (tcb == NULL) {
        return NULL;
    }
	tcb->stack_base = stack_base;
	tcb->exited = FALSE;
	int cond_ret = cond_init(&tcb->waiting_threads);
    if (cond_ret < 0) {
        free(tcb);
        return NULL;
    }
	int mutex_ret = mutex_init(&tcb->tcb_mutex);
    if (mutex_ret < 0) {
        free(tcb);
        return NULL;
    }
    return tcb;
}

/** @brief Function to add a TCB to the TCB list
 *
 *  Calls to this function are not thread safe and must be protected
 *  by a mutex. This function sets the tid in the TCB.
 *
 *  @param tid the tid of the TCB being added
 *  @param tcb pointer to the sruct holding the TCB data
 *
 *  @return tcb_t * return pointer to an initialized tcb or NULL if it fails
 */
void add_tcb(int tid, tcb_t *tcb) {
    tcb->id = tid;
	add_to_tail(&tcb->tcb_list, &head.tcb_list);
}

/** @brief Function to remove an entry from the list of TCBs
 *
 *  @param tcb TCB to be removed
 *
 *  @return Void
 */
void remove_tcb(tcb_t *tcb) {	
	del_entry(&tcb->tcb_list);
	mutex_destroy(&tcb->tcb_mutex);
	cond_destroy(&tcb->waiting_threads);
	free(tcb);
}
