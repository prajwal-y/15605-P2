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

/** @brief initialize a mutex
 *
 *  Set the mutex value to 1 indicating that it is unlocked
 *  and initialize the wait queue. Initializing a mutex after
 *  intializing it sets the value to 1 and "unlocks" it. Depending
 *  on if another thread holds the lock currently, this can lead to
 *  undefined behavior. Th wait queue is also emptied leaving tasks 
 *  stranded in the WAITING state.
 *
 *  @return 0 on success and -1 for failure
 */
int mutex_init(mutex_t *mp) {
    if (mp == NULL) {
        return -1;
    }
    mp->value = 1;
    init_head(&mp->waiting);
    return 0;
}

/** @brief destroy a mutex
 *
 *  Sets the value of the mutex to -1 indicating that it is inactive.
 *  If the value < 1 indicates that either locked or locked and have waiting 
 *  threads. In this case mutex_destroy does not deactivate the mutex. This 
 *  function does not reclaim the space used by the mutex.
 *  @return void
 */
void mutex_destroy(mutex_t *mp) {
    mp->value = -1;
}

/** @brief attempt to acquire the lock
 *
 *  We use the x86 xchg command to atomically test and set the value of the mutex.
 *  if the value is less than 1, we add the thread to the wait list of the 
 *  mutex and yield.
 *
 *  @return void
 */
void mutex_lock(mutex_t *mp) {
    int curr = test_and_unset(&mp->value);
    if (curr == 0) { /* Was currently locked so let someone else work */
        /* TODO: Add TCB to wait list */
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
    mp->value = 1;
    /* TODO: make_runnable first thread in wait list */
}
