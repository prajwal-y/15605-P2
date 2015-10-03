/** @file cond_type.h
 *  @brief This file defines the type for condition variables.
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H


typedef struct cond {
    int status;
    int signal_count;
    list_head waiting;
    mutex_t queue_mutex;
} cond_t;

#endif /* _COND_TYPE_H */
