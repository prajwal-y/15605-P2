/** @file cond_var.c
 *  @brief Implementation of condition variable calls
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <cond.h>
#include <asm.h>
#include <list.h>
#include <syscall.h>
#include <errors.h>
#include <malloc.h>
#include <mutex.h>
#include <simics.h>

/** @brief a struct to keep track of queue of threads blocked on something */
typedef struct blocked_thread {
    int tid;
    list_head link;
} blocked_thread_t;

/** @brief initialize a cond var
 *
 *  Set status of cond var to 1. It "initializes" the mutex pointed to 
 *  by cv. This also initializes the head of queue of waiting threads. We also
 *  keep track of signals that happen before a wait by counting signals
 *  when the queue is empty. This way we avoid the lost signal problem. 
 *
 *  @param cv a pointer to the condition variable
 *  @return 0 on success and ERR_INVAL for invalid input
 */
int cond_init(cond_t *cv) {
    if (cv == NULL) {
        return ERR_INVAL;
    }
    cv->status = 1;
    cv->signal_count = 0;
    if (mutex_init(&cv->queue_mutex) < 0) {
        return ERR_INVAL;
    }
    init_head(&cv->waiting);
    return 0;
}

/** @brief destroy a cond var
 *
 *  Sets the value of the status to -1 indicating that it is inactive.
 *  This function does not reclaim the space used by the cond var.
 *
 *  @param cv a pointer to the condition variable
 *  @return void
 */
void cond_destroy(cond_t *cv) {
    if (cv == NULL) {
        return;
    }
    mutex_destroy(&cv->queue_mutex);
    cv->status = 0;
}

/** @brief This function allows a thread to sleep on a signal issued on 
 *         some condition
 *
 *  We check if there has been a call to cond_signal before cond_wait was 
 *  called by checking the signal_count value. If there has been a signal we
 *  just return. If not we add ourselves to the queue and attempt deschedule 
 *  ourselves. The deschedule has to happen after we unlock mutexes and as such
 *  there is a potential race condition which is handled by going in a loop 
 *  and checking the value of signal_count. This while loop also ensures that
 *  one cond_signal wakes up only one waiting thread.
 *
 *  @pre the mutex pointed to by mp must be locked
 *  @post the mutex pointed to by mp is unlocked
 *  @param cv a pointer to the condition variable
 *  @param mp a pointer to the mutex associated with the thread
 *  @return void
 */
void cond_wait(cond_t *cv, mutex_t *mp) {
    /* Mutex to enure cond_wait and cond_join are not interleaved 
     * in a bad way*/
    mutex_lock(&cv->queue_mutex);

    /* Check for stored signals */
    if(cv->signal_count > 0) {
        cv->signal_count--;
        mutex_unlock(mp);
        mutex_unlock(&cv->queue_mutex);
        return;
    }

    int tid = gettid();
    blocked_thread_t *t = (blocked_thread_t *)
                            malloc(sizeof(blocked_thread_t));
    t->tid = tid;
    add_to_tail(&t->link, &cv->waiting);
    mutex_unlock(mp);
    mutex_unlock(&cv->queue_mutex);

	while(1) {
    	deschedule(&cv->signal_count);
		mutex_lock(&cv->queue_mutex);
		if(cv->signal_count > 0) {
			cv->signal_count--;
			mutex_unlock(&cv->queue_mutex);
			break;
		}
		mutex_unlock(&cv->queue_mutex);
	}
}


/** @brief this function signals an event and wakes up a waiting thread
 *         if present
 *
 *  Grab a mutex to prevent unfortunate interleavings of this function 
 *  with other cond var functions. Increment a variable to avoid the lost
 *  signal problem. If the queue of currently waiting threads is not empty
 *  we make_runnable that thread and free the struct associated with that 
 *  struct.
 *
 *  @param cv a pointer to the condition variable
 *  @return void
 */
void cond_signal(cond_t *cv) {
	mutex_lock(&cv->queue_mutex);
	cv->signal_count++;
    
    list_head *waiting_thread = get_first(&cv->waiting);
    if (waiting_thread != NULL) {
        blocked_thread_t *thr = get_entry(waiting_thread, blocked_thread_t, 
                                          link);
        del_entry(waiting_thread);
        int next_tid = thr->tid;
        free(thr);
        make_runnable(next_tid);
    }

    mutex_unlock(&cv->queue_mutex);
}
