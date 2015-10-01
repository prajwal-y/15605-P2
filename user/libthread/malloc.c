/**
 * @file malloc.c
 * @brief This file implements the thread safe functions
 * for malloc, calloc, realloc and free.
 */

#include <stdlib.h>
#include <types.h>
#include <stddef.h>
#include <mutex.h>

static mutex_t mutex;
static int initialized = 0;

void init_mutex() {
	mutex_init(&mutex);
	initialized = 1;
}

/**
 * @brief Thread safe malloc function. This function is a wrapper
 * around _malloc() function, using a mutex.
 *
 * @param __size Memory to be allocated
 *
 * @return Void 
 */
void *malloc(size_t __size) {
	if(!initialized) {
		init_mutex();
	}
	mutex_lock(&mutex);
	void * allocated =  _malloc(__size);
	mutex_unlock(&mutex);
	return allocated;
}

void *calloc(size_t __nelt, size_t __eltsize) {
	if(!initialized) {
		init_mutex();
	}
	mutex_lock(&mutex);
	void * allocated =  _calloc(__nelt, __eltsize);
	mutex_unlock(&mutex);
	return allocated;
}

void *realloc(void *__buf, size_t __new_size) {
	if(!initialized) {
		init_mutex();
	}
	mutex_lock(&mutex);
	void * allocated =  _realloc(__buf, __new_size);
	mutex_unlock(&mutex);
	return allocated;
}

void free(void *__buf) {
	if(!initialized) {
		init_mutex();
	}
	mutex_lock(&mutex);
	_free(__buf);
	mutex_unlock(&mutex);
}
