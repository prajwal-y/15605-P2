/** @file fork.S
 *  @brief Stub routine for the fork system call
 *  
 *  Calls the fork system call by calling INT FORK_INT with
 *  no parameters.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global fork

fork:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */

    /* Body */
    int $FORK_INT

    /* Finish */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
