/** @file tcb.h
 *  @brief This file defines the type for task control blocks.
 */

#ifndef __TCB_H
#define __TCB_H
#include <list.h>

/** @brief enum type for the state of a task.
 */
typedef enum {
    BLOCKED,
    RUNNABLE,
    RUNNING,
    STOPPED
} task_state;


/** @brief a type for task control blocks.
 *
 *  At any given point in time a thread can be waiting to acquire only 
 *  ONE resource. Therefore wait_q must either be part of 
 *  a single wait queue or must not be linked at all. Being part
 *  of multiple wait queue can lead to bad things happening.
 */
typedef struct tcb {
    int id;
    list_head wait_queue;  /* The wait queue for some resource */
    task_state state;
} tcb_t;

#endif /* __MUTEX_TYPE_H */
