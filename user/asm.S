/** user/asm.S 
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapdithaya (pyadapad)
 */

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
