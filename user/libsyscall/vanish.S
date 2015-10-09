/** @file vanish.S
 *  @brief Stub routine for the vanish system call
 *  
 *  Calls the vanish system call by calling INT VANISH_INT with no parameters.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall_int.h>

.global vanish

vanish:
    /* Body */
    int $VANISH_INT

    /* Finish */
    ret                 
