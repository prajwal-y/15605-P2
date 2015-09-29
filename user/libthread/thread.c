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

/**
 * @brief This function is responsible for initializing the
 * thread library.
 *
 * @param size Size of the stack space available for each thread
 * 
 * @return int 0 if initialization is successful. -1 otherwise
 */
int thr_init(unsigned int size) {
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
	return 0;
}

/**
 * @brief This function is responsible for "cleaning up" a thread with
 * given tid, optionally  returning  the  status  information provided 
 * by the thread at the time of exit. If the thread is not exited, it
 * will be suspended until the thread is exited.
 *
 * @param tid Thread ID of the thread to be cleaned up.
 * @param statusp The value passed to thr_exit() by the joined thread 
 * will be placed in the location referenced by statusp.
 *
 * @return int 0 on success, error code (negative number) on error 
 */
int thr_join(int tid, void **statusp) {
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
}

/**
 * @brief This function returns the thread ID of the current thread
 *
 * @return int The thread ID of the current thread.
 */
int thr_getid() {
	return 0;
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
	return 0;
}
