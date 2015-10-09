/** @file autostack.c
 *  @brief Grow the stack
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall.h>
#include <ureg.h>
#include <malloc.h>
#include <simics.h>
#include <asm.h>
#include <panic.h>

#define EXCEPTION_STACK_SIZE 1024
void *stack_bottom;
void *exception_stack;

/** @brief software exception handler
 *  
 *  Called by the kernel when an exception occurs and a software 
 *  exception handler is installed. The exception handler currently
 *  checks the cause of the exception and if the cause is a page fault,
 *  it attempts to grow the stack. If the new_pages call fails for any reason
 *  (insufficient resources) then we panic and crash the program, since there
 *  is no way we can recover from this.
 *
 *  @param arg arguments to the exception handler
 *  @param ureg the current values of the register set
 *
 *  @return void
 */
void seh(void *arg, ureg_t *ureg) {
    if (ureg->cause == SWEXN_CAUSE_PAGEFAULT) {
        int ret = new_pages(stack_bottom - PAGE_SIZE, PAGE_SIZE);
        if (ret < 0) {
            die("Stack overflow error!");
        }
        stack_bottom -= PAGE_SIZE;
        swexn((char *)exception_stack + EXCEPTION_STACK_SIZE, 
              seh, NULL, ureg);
    }
}

/** @brief install the auto stack growing handler
 *  
 *  Called by the main wrapper before our program's main is called.
 *  Initializes the exception stack where the software exception handler will
 *  run and makes a call to swexn to install the handler. If we are unable to
 *  malloc enough space for the exception handler, we simply return from the 
 *  function without installing the exception handler. In case a page fault 
 *  exception does occur in that case since we do not have an exception handler 
 *  installed the program will crash which is acceptable behavior given the
 *  circumstances.
 *
 *  @param stack_high the highest valid address of the stack
 *  @param stack_low the lowest valid address of the stack
 *
 *  @return void
 */
void install_autostack(void *stack_high, void *stack_low) {
    stack_bottom = stack_low;
    exception_stack = malloc(EXCEPTION_STACK_SIZE);
    if (exception_stack == NULL) {
        return;
    }
    swexn((char *)exception_stack + EXCEPTION_STACK_SIZE, 
          seh, NULL, 0);
}

/** @brief deregister the swexn handler
 *  
 *  Deregister by calling the swexn system call without parameters.
 *
 *  @return void
 */
void uninstall_seh() {
    swexn(NULL, NULL, NULL, 0);
}

/** @brief the exception handler for the multi threaded scenario
 *  
 *  panic and kill the program.
 *
 *  @return void
 */
void seh_multi(void *arg, ureg_t *ureg) {
    die("Thread caused a segmentation fault.");
}

/** @brief install the exception handler for the multi threaded scenario
 *  
 *  called for each thread which is created. If malloc fails we return and
 *  hope that no thread overflows the stack. If it does then we rely on the
 *  default kernel exception handler to crash the program.
 *
 *  @return void
 */
void install_seh_multi() {
    exception_stack = malloc(EXCEPTION_STACK_SIZE);
    if (exception_stack == NULL) {
        return;
    }
    swexn((char *)exception_stack + EXCEPTION_STACK_SIZE, 
          seh_multi, NULL, 0);
}
    
