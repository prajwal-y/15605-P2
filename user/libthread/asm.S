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

.global thread_fork
thread_fork:
	pushl %ebx
	movl 8(%esp), %ebx /*Save the new %esp value*/
	movl 12(%esp), %ecx /*Save the address of func*/
	movl 16(%esp), %edx /*Save the address of argument to func*/
    int $THREAD_FORK_INT
	cmpl $0, %eax
	je handle_child_thr
	popl %ebx
    ret
     
handle_child_thr:
	movl %ebx, %esp      /*Update %esp of the new thread stack*/
	pushl %edx           /*Push the arguments to func*/
    pushl %ecx           /* Address of the thread function */
	call new_thread_init /* Call the new thread wrapper */
	ret                  /* Should never come here */
