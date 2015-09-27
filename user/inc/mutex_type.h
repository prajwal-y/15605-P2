/** @file mutex_type.h
 *  @brief This file defines the type for mutexes.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H
#include <list.h>

typedef struct mutex {
    int value;          /* Will be 0 or 1 */
    list_head waiting;  /* List of processes waiting for lock */
} mutex_t;

#endif /* _MUTEX_TYPE_H */
