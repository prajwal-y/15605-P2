/** @file tcb.h
 *  @brief This file defines the type for task control blocks.
 */

#ifndef __TCB_H
#define __TCB_H
#include <list.h>
#include <cond_type.h>


/** @brief a type for task control blocks.
 *
 *  At any given point in time a thread can be waiting to acquire only 
 *  ONE resource. Therefore wait_queue must either be part of 
 *  a single wait queue or must not be linked at all. Being part
 *  of multiple wait queue can lead to bad things happening.
 */
typedef struct tcb {
    int id;
	int exited;
	void *stack_base;
	void *status;
    list_head tcb_list;
    cond_t waiting_threads;   /* For threads joining on this thread */
	mutex_t tcb_mutex;
} tcb_t;

#endif /* __TCB_H */
