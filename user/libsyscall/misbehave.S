/** @file misbehave.S
 *  @brief Stub routine for the misbehave system call
 *  
 *  Calls the misbehave system call by calling INT MISBEHAVE_INT with
 *  the parameters. The single parameter is stored in ESI.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global misbehave

misbehave:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */
    pushl %esi           /* Callee save register */

    /* Body */
    movl 8(%ebp),%esi   /* Store argument in esi */
    int $MISBEHAVE_INT

    /* Finish */
    movl -4(%ebp),%esi  /* Restore ESI */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
