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
#include <simics.h>

/** @brief initialize a mutex
 *
 *  Set the mutex value to 1 indicating that it is unlocked
 *  and initialize the wait queue. Initializing a mutex after
 *  initializing it sets the value to 1 and "unlocks" it. Depending
 *  on if another thread holds the lock currently, this can lead to
 *  undefined behavior. 
 *
 *  @return 0 on success and -1 for invalid input
 */
int mutex_init(mutex_t *mp) {
    if (mp == NULL) {
        return ERR_INVAL;
    }
    mp->value = MUTEX_VALID;
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
    mp->value = MUTEX_INVALID;
}

/** @brief attempt to acquire the lock
 *
 *  We use the x86 xchg command to atomically test and set the value of the mutex.
 *  Spin lock till we get the lock. Theoretically this does not provide bounded
 *  waiting. But we are on a uniprocessor environment assuming a scheduler that
 *  does not starve any process and is fair. Assuming this and given that the
 *  number of instructions required to test and get the lock is small, once a 
 *  lock is released, it is highly likely that the next thread (that is fairly
 *  scheduled by the kernel) will get the lock. By fair scheduling we mean that
 *  no thread will wait forever (maybe something as simple as a round robin
 *  scheduler).
 *
 *  If the mutex is corrupted or destroyed, calling this function will result 
 *  in undefined behaviour
 *
 *  @return void
 */
void mutex_lock(mutex_t *mp) {
    while (!test_and_unset(&mp->value));
}

/** @brief release a lock
 *
 *  If the mutex is corrupted or destroyed, calling this function will result 
 *  in undefined behaviour
 *
 *  @return void
 */
void mutex_unlock(mutex_t *mp) {
    test_and_set(&mp->value);
}
