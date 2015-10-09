/** @file asm.S 
 *  
 *  Assembly wrappers for functions used by the thread library
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapdithaya (pyadapad)
 */
#include <syscall_int.h>

.global test_and_unset
test_and_unset:
    movl 4(%esp), %ecx  /*Get the parameter value (address of mutex variable)*/
    movl $0, %eax		/*Set 0 to eax*/
    xchg (%ecx), %eax	/*Atomically exchange the value*/
    ret
    
.global test_and_set
test_and_set:
    movl 4(%esp), %ecx	/*Get the parameter value (address of mutex variable)*/
    movl $1, %eax		/*Set 1 to eax*/
    xchg (%ecx), %eax	/*Atomically exchange the value*/
    ret

.global thread_fork
thread_fork:
	pushl %ebx
	movl 8(%esp), %ebx		/*Save the new %esp value*/
	movl 12(%esp), %ecx		/*Save the address of func*/
	movl 16(%esp), %edx		/*Save the address of argument to func*/
    int $THREAD_FORK_INT	/*Invoke therad_fork system call*/
	cmpl $0, %eax			/*If eax == 0 , then it's child thread*/
	je handle_child_thr		/*Jump to child thread handler*/
	popl %ebx				/*Restore the parent thread ebx*/
    ret
     
handle_child_thr:
	movl %ebx, %esp      /*Update %esp of the new thread stack*/
	pushl %edx           /*Push the arguments to func*/
    pushl %ecx           /* Address of the thread function */
	call new_thread_init /* Call the new thread wrapper */
	ret                  /* Should never come here */
