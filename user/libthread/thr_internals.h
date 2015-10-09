/** @file thr_internals.h
 *  @brief This file has functions internal to the thread library
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

void new_thread_init(void *(*func_addr)(void *), void *arg);

#endif /* THR_INTERNALS_H */
