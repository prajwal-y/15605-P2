/** @file set_term_color.S
 *  @brief Stub routine for the set terminal color system call
 *  
 *  Calls the set terminal color system call by calling INT SET_TERM_COLOR_INT with
 *  the parameters. The single parameter is stored in ESI.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global set_term_color

set_term_color:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */
    pushl %esi           /* Callee save register */

    /* Body */
    movl 8(%ebp),%esi   /* Store argument in esi */
    int $SET_TERM_COLOR_INT

    /* Finish */
    movl -4(%ebp),%esi  /* Restore ESI */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
