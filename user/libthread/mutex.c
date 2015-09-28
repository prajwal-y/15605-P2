/** @file mutex.c
 *  @brief Implementation of mutex calls
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <mutex.h>
#include <asm.h>
#include <list.h>
#include <syscall.h>
#include <errors.h>
#include <malloc.h>

typedef struct blocked_thread {
    int tid;
    list_head link;
} blocked_thread_t;

/** @brief initialize a mutex
 *
 *  Set the mutex value to 1 indicating that it is unlocked
 *  and initialize the wait queue. Initializing a mutex after
 *  intializing it sets the value to 1 and "unlocks" it. Depending
 *  on if another thread holds the lock currently, this can lead to
 *  undefined behavior. 
 *
 *  @return 0 on success and -1 for invalid input
 */
int mutex_init(mutex_t *mp) {
    if (mp == NULL) {
        return ERR_INVAL;
    }
    mp->value = 1;
    init_head(&mp->waiting);
    return 0;
}

/** @brief destroy a mutex
 *
 *  Sets the value of the mutex to -1 indicating that it is inactive.
 *  This function does not reclaim the space used by the mutex.
 *
 *  @return void
 */
void mutex_destroy(mutex_t *mp) {
    if (mp == NULL) {
        return;
    }
    mp->value = -1;
}

/** @brief attempt to acquire the lock
 *
 *  We use the x86 xchg command to atomically test and set the value of the mutex.
 *  if the value is 0, we add the thread to the wait list of the 
 *  mutex and deschedule.
 *
 *  @return void
 */
void mutex_lock(mutex_t *mp) {
    int curr = test_and_unset(&mp->value);
    if (curr == 0) { /* Was currently locked so let someone else work */
        blocked_thread_t *t = (blocked_thread_t *)
                                malloc(sizeof(blocked_thread_t));
        t->tid = gettid();
        add_to_tail(&t->link, &mp->waiting);
        int reject = 0;
        deschedule(&reject);
    }
}

/** @brief release a lock
 *
 *  Set the value of mutex and make_runnable the first
 *  thread in the wait list
 *
 *  @return void
 */
void mutex_unlock(mutex_t *mp) {
    list_head *waiting_thread = get_first(&mp->waiting);
    if (waiting_thread != NULL) {
        del_entry(waiting_thread);
        int next_tid = waiting_thread->tid;
        free(waiting_thread);
        make_runnable(next_tid);
    } else {
        mp->value = 1;
    }
}
