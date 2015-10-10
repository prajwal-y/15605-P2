/** @file rwlock.c
 *  @brief Implementation of read write lock functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <cond.h>
#include <mutex.h>
#include <rwlock.h>
#define RWLOCK_FREE 2
#define RWLOCK_INVALID -1

/** @brief function to initialize a read-write lock
 *
 *  this function initializes the cond vars associated with 
 *  the readers and writers and the mutex.
 *
 *  @param rwlock the rwlock to initialize
 *  @return int 0 on success -1 on failure
 */
int rwlock_init(rwlock_t *rwlock) {
    mutex_init(&rwlock->mutex);
    cond_init(&rwlock->readers);
    cond_init(&rwlock->writers);
    rwlock->type = RWLOCK_FREE;
    rwlock->num_writers = 0;
    rwlock->curr_readers = 0;
    return 0;
}

/** @brief lock the read write lock
 *
 *  If the lock requested is a write lock we increment the number of writers
 *  and check if we can get the lock. We can get the lock either if the lock 
 *  is currently free or if there are no readers currently reading the state 
 *  being locked. Increasing the number of writers enures that future readers
 *  and writers wait till the writers are done. Any read lock has to wait when
 *  the lock is in write mode or the number of writers is greater than 0.
 *
 *  @param rwlock the rwlock to lock
 *  @param type the type of lock being requested
 *  @return void
 */
void rwlock_lock(rwlock_t *rwlock, int type) {
    if (rwlock == NULL || (type != RWLOCK_READ && type != RWLOCK_WRITE)) {
        return;
    }
    mutex_lock(&rwlock->mutex);
    if (type == RWLOCK_WRITE) {
        rwlock->num_writers++;
        while (rwlock->type != RWLOCK_FREE && rwlock->curr_readers > 0) {
            cond_wait(&rwlock->writers, &rwlock->mutex);
        }
        rwlock->type = RWLOCK_WRITE;
    } 
    else if (type == RWLOCK_READ) {
        while (rwlock->type == RWLOCK_WRITE || rwlock->num_writers > 0) {
            cond_wait(&rwlock->readers, &rwlock->mutex);
        }
        rwlock->curr_readers++;
        rwlock->type = RWLOCK_READ;
    }
    mutex_unlock(&rwlock->mutex);
}

/** @brief unlock the read write lock
 *
 *  If the lock is currently held by a writer and is unlocked we reduce 
 *  the number of writers and set the type to free. We also broadcast to
 *  all readers and signal any thread waiting for a write lock. If there
 *  are other writers waiting for the lock readers promptly go back to sleep
 *  and the signal ensures atleast one writer is awoken if present. If a read
 *  lock is being unlocked, we reduce the number of readers and again broadcast 
 *  to all readers and signal a writer. We broadcast to all readers since we 
 *  allow multiple readers to acquire the lock.
 *
 *  @param rwlock the rwlock to unlock
 *  @return void
 */
void rwlock_unlock(rwlock_t *rwlock) {
    if (rwlock == NULL) {
        return;
    }
    mutex_lock(&rwlock->mutex);
    if (rwlock->type == RWLOCK_WRITE) {
        rwlock->num_writers--;
        rwlock->type = RWLOCK_FREE;
        cond_broadcast(&rwlock->readers);
        cond_signal(&rwlock->writers);
    } 
    else if (rwlock->type == RWLOCK_READ) { 
        rwlock->curr_readers--;
        if (rwlock->curr_readers == 0) {
            rwlock->type = RWLOCK_FREE;
            cond_broadcast(&rwlock->readers);
            cond_signal(&rwlock->writers);
        }
    }
    mutex_unlock(&rwlock->mutex);
}

/** @brief destroy the read write lock
 *
 *  Invalidate the lock by setting the type to an invalid value.
 *
 *  @param rwlock the rwlock to unlock
 *  @return void
 */
void rwlock_destroy(rwlock_t *rwlock ) {
    rwlock->type = RWLOCK_INVALID;
    return;
}

/** @brief downgrade the read write lock
 *
 *  This function does nothing if the lock is currently held by a reader.
 *  If it is currently held by a writer we reduce the number of writers 
 *  and broadcast to all readers and make the lock as read. This ensures 
 *  that all the readers currently waiting and the calling thread acquire
 *  the read lock. Future writers have to wait for these readers to release
 *  the lock.
 *
 *  @param rwlock the rwlock to unlock
 *  @return void
 */
void rwlock_downgrade(rwlock_t *rwlock) {
    if (rwlock == NULL) {
        return;
    }
    mutex_lock(&rwlock->mutex);
    if (rwlock->type != RWLOCK_WRITE) {
        mutex_unlock(&rwlock->mutex);
        return;
    }
    rwlock->num_writers--;
    rwlock->curr_readers++;
    rwlock->type = RWLOCK_READ;
    cond_broadcast(&rwlock->readers);
    mutex_unlock(&rwlock->mutex);
}
