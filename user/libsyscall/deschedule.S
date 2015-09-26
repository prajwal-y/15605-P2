/** @file deschedule.S
 *  @brief Stub routine for the deschedule system call
 *  
 *  Calls the deschedule system call by calling INT DESCHEDULE_INT with
 *  the parameters. The single parameter is stored in ESI.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global deschedule

deschedule:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */
    pushl %esi           /* Callee save register */

    /* Body */
    movl 8(%ebp),%esi   /* Store argument in esi */
    int $DESCHEDULE_INT

    /* Finish */
    movl -4(%ebp),%esi  /* Restore ESI */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
