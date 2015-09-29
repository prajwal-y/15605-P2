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

typedef struct blocked_thread {
    int tid;
    list_head link;
} blocked_thread_t;

/** @brief initialize a cond var
 *
 *  Set status of cond var to 0. It "initializes" the cond var pointed to 
 *  by cv. This also initializes the queue of waiting threads. 
 *
 *  @param cv a pointer to the condition variable
 *  @return 0 on success and -1 for invalid input
 */
int cond_init(cond_t *cv) {
    if (cv == NULL) {
        return ERR_INVAL;
    }
    cv->status = 1;
    mutex_init(&cv->queue_mutex);
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

/** @brief This function allows a thread to wait for a function and releases
 *         the mutex that it needs to check that condition. The blocked thread
 *         may be awakened by a call to cond_signal or cond_broadcast
 *
 *  @param cv a pointer to the condition variable
 *  @param mp a pointer to the mutex associated with the thread
 *  @return void
 */
void cond_wait(cond_t *cv, mutex_t *mp) {
    /* Create a blocked task struct */
    int tid = gettid();
    blocked_thread_t *t = (blocked_thread_t *)
                            malloc(sizeof(blocked_thread_t));
    t->tid = tid;
    /* Atomically add ourselves to the queue */
    mutex_lock(&cv->queue_mutex);
    add_to_tail(&t->link, &cv->waiting);
    cv->reject = 0;
    mutex_unlock(&cv->queue_mutex);

    mutex_unlock(mp);
    deschedule(&(cv->reject));

    mutex_lock(mp);
}

/** @brief this function is called by a thread which wishes to signal
 *         the occurence of an event to a thread which is waiting for 
 *         the event. The thread is awoken and we set the reject value 
 *         to be 1.
 *
 *  @param cv a pointer to the condition variable
 *  @return void
 */
void cond_signal(cond_t *cv) {
    /* Atomically set the value of reject to 1 to prevent any thread waiting 
     * for us to be descheduled after the signal and hence losing the signal
     * forever. Also get the first waiting thread and make it runnable.
     */
	mutex_lock(&cv->queue_mutex);
	cv->reject = 1;
    list_head *waiting_thread = get_first(&cv->waiting);
    mutex_unlock(&cv->queue_mutex);
    
    if (waiting_thread != NULL) {
        blocked_thread_t *thr = get_entry(waiting_thread, blocked_thread_t, 
                                          link);
        del_entry(waiting_thread);
        int next_tid = thr->tid;
        free(thr);
        make_runnable(next_tid);
    }
}

