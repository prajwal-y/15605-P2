/** @file halt.S
 *  @brief Stub routine for the halt system call
 *  
 *  Calls the halt system call by calling INT HALT_INT with no parameters.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global halt

halt:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */

    /* Body */
    int $HALT_INT

    /* Finish */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
