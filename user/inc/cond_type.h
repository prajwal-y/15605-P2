/** @file cond_type.h
 *  @brief This file defines the type for condition variables.
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H

#define COND_VAR_VALID 1
#define COND_VAR_INVALID 0

typedef struct cond {
    int status;
    list_head waiting;
    mutex_t queue_mutex;
} cond_t;

#endif /* _COND_TYPE_H */
