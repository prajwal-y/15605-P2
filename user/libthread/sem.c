/** @file sem.c
 *  @brief Implementation of semaphores
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <cond.h>
#include <mutex.h>
#include <sem.h>
#include <simics.h>
#define RWLOCK_FREE 2

/** @brief This function will initialize a semaphore to
 * 	a given value count. 
 *
 *  @param sem Semaphore to be initialized
 *  @param count Count of the semaphore
 *  @return int 0 on success, error code on failure
 */
int sem_init(sem_t *sem, int count) {
	if(sem == NULL || count <= 0) {
		return -1;
	}
	mutex_init(&sem->mutex);
	cond_init(&sem->cond_var);
	sem->count = count;
	sem->valid = 1;
	return 0;
}

/**
 * @brief This function allows the semaphore to be decremented.
 * The function blocks until the count of the semaphore is
 * valid to be decremented.
 *
 * @param sem Semaphore whose count should be decremented.
 * @return Void
 */
void sem_wait(sem_t *sem) {
    if (sem == NULL || (!sem->valid)) {
        return;
    }
    mutex_lock(&sem->mutex);
	while(sem->count == 0) {
		cond_wait(&sem->cond_var, &sem->mutex);
	}
	sem->count--;
    mutex_unlock(&sem->mutex);
}

/**
 * @brief Function to wake up a thread waiting on the 
 * semaphore. The value of the count of the semaphore
 * is incremented before signaling the waiting threads.
 *
 * @param sem Semaphore whose count needs to be increased
 * @return Void
 */
void sem_signal(sem_t *sem) {
    if (sem == NULL || (!sem->valid)) {
        return;
    }
    mutex_lock(&sem->mutex);
	sem->count++;
	cond_signal(&sem->cond_var);
    mutex_unlock(&sem->mutex);
}

/**
 * @brief Function to deactivate a semaphore.
 *
 * @param sem Semaphore which needs to be deactivated
 * @return Void
 */
void sem_destroy(sem_t *sem) {
    sem->valid = 0;
    return;
}
