/** @file sem_type.h
 *  @brief This file defines the type for semaphores.
 */

#ifndef _SEM_TYPE_H
#define _SEM_TYPE_H

#include <cond.h>
#include <mutex.h>

typedef struct sem {
	mutex_t mutex;
	cond_t cond_var;
	int count;
	int valid;
} sem_t;

#endif /* _SEM_TYPE_H */
