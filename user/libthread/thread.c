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

#define STACK_PADDING(size) ((((size)%4)==0)?0:(4-((size)%4)))

static unsigned int stack_size;
static tcb_t head;
static volatile int count1 = 0;
static volatile int count2 = 0;

static mutex_t tcb_lock;

/*Helper functions*/
static tcb_t *find_tcb(int tid);
static void remove_tcb(tcb_t *tcb);
static void add_tcb(int tid, void *stack_base);

/**
 * @brief This function is responsible for initializing the
 * thread library.
 *
 * @param size Size of the stack space available for each thread
 * 
 * @return int 0 if initialization is successful. -1 otherwise
 */
int thr_init(unsigned int size) {
    uninstall_seh();
    stack_size = size + STACK_PADDING(size);
	mutex_init(&tcb_lock);
    init_head(&head.tcb_list);

	/*Add the current thread to the TCB list*/
	mutex_lock(&tcb_lock);
	add_tcb(thr_getid(), NULL);
	mutex_unlock(&tcb_lock);

	return 0;
}

/**
 * @brief This function is responsible for creating a thread to 
 * run the function func(arg).
 *
 * @param func Function pointer to the function that the thread
 * needs to run
 * @param arg Parameters to the function to be called by the thread
 *
 * @return int If thread creation is successful, the thread ID of the 
 * new thread is returned. Otherwise, -1 is returned.
 */
int thr_create(void *(*func)(void *), void *arg) {
    char *stack_base = ((char *)malloc(stack_size));
    if (stack_base == NULL) {
        return ERR_NOMEM;
    }
	mutex_lock(&tcb_lock); /*Lock the TCB list for adding a TCB entry*/
	int tid = thread_fork((stack_base + stack_size), func, arg);
	add_tcb(tid, stack_base);
	mutex_unlock(&tcb_lock);

	return tid;
}

/**
 * @brief This function is responsible for "cleaning up" a thread with
 * given tid, optionally returning the status information provided by 
 * the thread at the time of exit. If the thread is not exited, it
 * will be suspended until the thread is exited.
 *
 * @param tid Thread ID of the thread to be cleaned up.
 * @param statusp The value passed to thr_exit() by the joined thread 
 * will be placed in the location referenced by statusp.
 *
 * @return int 0 on success, error code (negative number) on error 
 */
int thr_join(int tid, void **statusp) {
	if(tid < 0 || statusp == NULL) {
		return ERR_INVAL;
	}
	mutex_lock(&tcb_lock);
	tcb_t *tcb = find_tcb(tid);
	mutex_unlock(&tcb_lock);
    
	assert(tcb != NULL);
	mutex_lock(&tcb->tcb_mutex);
	while(tcb->exited != 1) {
		cond_wait(&tcb->waiting_threads, &tcb->tcb_mutex);
		mutex_lock(&tcb->tcb_mutex);
	}
	
	*statusp = tcb->status;
	if(tcb->stack_base != NULL) {
		free(tcb->stack_base);
	}
	
	mutex_lock(&tcb_lock);
	remove_tcb(tcb);
	mutex_unlock(&tcb_lock);
	
	return 0;
}

/**
 * @brief This function exits the thread with exit status.
 *
 * @param status The exit status of the thread.
 *
 * @return Void 
 */
void thr_exit(void *status) {
	int tid = thr_getid();

	mutex_lock(&tcb_lock);
	tcb_t *tcb = find_tcb(tid);
	mutex_unlock(&tcb_lock);

	assert(tcb != NULL);
	tcb->exited = 1;
	tcb->status = status;
	cond_signal(&tcb->waiting_threads);
	vanish();
}

/**
 * @brief wrapper function to install exception handler for new thread
 *        and call thread function
 *
 * @param func_addr address of thread function
 * @param arg arguments to thread function
 * @return Void
 */
void new_thread_init(void *(*func_addr)(void *), void *arg) {	
    install_seh_multi();
    func_addr(arg);
    thr_exit((void *) 0);       /* in case thr_exit not called by programmer */
}

/**
 * @brief This function returns the thread ID of the current thread
 *
 * @return int The thread ID of the current thread.
 */
int thr_getid() {
	return gettid(); //TODO: FIX THIS
}

/**
 * @brief Defers execution of the invoking thread to a later time in
 * favor of the thread with ID.
 *
 * @param tid Thread ID of the thread whose execution needs to be deferred
 *
 * @return int 0 if successful, 
 */
int thr_yield(int tid) {
	return yield(tid);
}

/**
 * @brief Function to linearly scan the list of TCBs for the TCB with the
 * given ID.
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

/**
 * @brief Function to add an entry to the list of TCBs
 *
 * @param stack_base Base address of the stack of the thread
 *
 * @return Void
 */
void add_tcb(int tid, void *stack_base) {
	tcb_t *tcb = (tcb_t *)malloc(sizeof(tcb_t));
	tcb->stack_base = stack_base;
	tcb->id = tid;
	tcb->exited = 0;
	tcb->reject = 0;
	cond_init(&tcb->waiting_threads);
	mutex_init(&tcb->tcb_mutex);
	add_to_tail(&tcb->tcb_list, &head.tcb_list);
}

/**
 * @brief Function to remove an entry from the list of TCBs
 *
 * @param tcb TCB to be removed
 *
 * @return Void
 */
void remove_tcb(tcb_t *tcb) {	
	del_entry(&tcb->tcb_list);
	mutex_unlock(&tcb->tcb_mutex);
	mutex_destroy(&tcb->tcb_mutex);
	cond_destroy(&tcb->waiting_threads);
	free(tcb);
}
