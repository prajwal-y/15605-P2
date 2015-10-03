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

#define EXCEPTION_STACK_SIZE 1024
void *stack_bottom;
void *exception_stack;

void seh(void *arg, ureg_t *ureg) {
    if (ureg->cause == SWEXN_CAUSE_PAGEFAULT) {
        new_pages(stack_bottom - PAGE_SIZE, PAGE_SIZE);
        stack_bottom -= PAGE_SIZE;
        swexn((char *)exception_stack + EXCEPTION_STACK_SIZE, 
              stack_overflow_handler, NULL, ureg);
    }
}

void install_autostack(void *stack_high, void *stack_low) {
    stack_bottom = stack_low;
    exception_stack = malloc(EXCEPTION_STACK_SIZE);
    swexn((char *)exception_stack + EXCEPTION_STACK_SIZE, 
          seh, NULL, 0);
}

void uninstall_seh() {
    swexn(NULL, NULL, NULL, 0);
}

void install_seh_multi() {
    stack_bottom = stack_low;
    exception_stack = malloc(EXCEPTION_STACK_SIZE);
    swexn((char *)exception_stack + EXCEPTION_STACK_SIZE, 
          seh_multi, NULL, 0);
}
    
void seh_multi(void *arg, ureg_t *ureg) {
    panic("%s", "No!!!");
}
