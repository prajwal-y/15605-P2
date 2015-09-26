/** @file gettid.S
 *  @brief Stub routine for the gettid system call
 *  
 *  Calls the gettid system call by calling INT GETTID_INT with no parameters.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global gettid

gettid:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */

    /* Body */
    int $GETTID_INT

    /* Finish */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
