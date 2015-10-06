/** @file rwlock_type.h
 *  @brief This file defines the type for reader/writer locks.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#ifndef _RWLOCK_TYPE_H
#define _RWLOCK_TYPE_H
#include <cond.h>
#include <mutex.h>

typedef struct rwlock {
    mutex_t mutex;
    cond_t readers;
    cond_t writers;
    int type;
    int num_writers;
    int curr_readers;
} rwlock_t;

#endif /* _RWLOCK_TYPE_H */
