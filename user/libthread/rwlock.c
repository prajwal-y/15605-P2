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

void rwlock_lock(rwlock_t *rwlock, int type) {
    if (rwlock == NULL || type < 0 || type > 1) {
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

void rwlock_destroy(rwlock_t *rwlock ) {
    rwlock->type = -1;
    return;
}

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
