/** user/asm.S 
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapdithaya (pyadapad)
 */
#include <syscall_int.h>

.global test_and_unset
test_and_unset:
    movl 4(%esp), %ecx
    movl $0, %eax
    xchg (%ecx), %eax
    ret
    
.global test_and_set
test_and_set:
    movl 4(%esp), %ecx
    movl $1, %eax
    xchg (%ecx), %eax
    ret

.global set_ebp
set_ebp:
    movl 4(%esp), %ebp
    ret

.global set_esp
set_esp:
    movl 4(%esp), %ecx
    movl (%esp), %edx
    movl %edx, -4(%ecx)
    addl $-4, %ecx
    movl %ecx, %esp
    ret

.global thread_fork
thread_fork:
    /* Setup */
    pushl %ebp          /* Old EBP */
    movl %esp,%ebp      /* New EBP */

    /* Body */
    int $THREAD_FORK_INT

    /* Finish */
    movl %ebp,%esp      /* Reset esp to start */
    popl %ebp           /* Restore ebp */
    ret                 
     
