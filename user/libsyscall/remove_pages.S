/** @file remove_pages.S
 *  @brief Stub routine for the set remove pages system call
 *  
 *  Calls the remove pages system call by calling INT REMOVE_PAGES_INT with
 *  the parameters. The single parameter is stored in ESI.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global remove_pages

remove_pages:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */
    pushl %esi           /* Callee save register */

    /* Body */
    movl 8(%ebp),%esi   /* Store argument in esi */
    int $REMOVE_PAGES_INT

    /* Finish */
    movl -4(%ebp),%esi  /* Restore ESI */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
