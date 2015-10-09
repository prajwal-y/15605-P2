/** @file panic.h
 *  @brief panicky functions
 *
 *  @author rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __PANIC_H
#define __PANIC_H

extern void panic_exit(const char *fmt, ...);

#define die(expression)  \
	((panic_exit("PANIC! `%s' at %s:%u." \
                 " Program will exit", expression, \
                 __FILE__, __LINE__)))

#endif  /* __PANIC_H */
