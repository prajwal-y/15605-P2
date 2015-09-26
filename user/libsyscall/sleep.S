/** @file sleep.S
 *  @brief Stub routine for the sleep system call
 *  
 *  Calls the sleep system call by calling INT SLEEP_INT with
 *  the parameters. The single parameter is stored in ESI.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global sleep

sleep:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */
    pushl %esi           /* Callee save register */

    /* Body */
    movl 8(%ebp),%esi   /* Store argument in esi */
    int $SLEEP_INT

    /* Finish */
    movl -4(%ebp),%esi  /* Restore ESI */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
