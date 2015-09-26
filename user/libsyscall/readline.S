/** @file readline.S
 *  @brief Stub routine for the readline system call
 *  
 *  Calls the readline system call by calling INT READLINE_INT with
 *  the parameters. Since there is more than one parameter we
 *  need to pass the address of a location having the parameters.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global readline

readline:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */
    pushl %esi           /* Callee save register */

    /* Body */
    movl %ebp,%esi   /* Move address ofebp to esi */
    add $8,%esi      /* We pass address of argument "packet" */
    int $READLINE_INT   

    /* Finish */
    movl -4(%ebp),%esi  /* Restore ESI */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret
