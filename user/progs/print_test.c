/** @file print_test.c
 *  @brief Test the print function call
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall.h>
#include <simics.h>
#include <contracts.h>
#include <panic.h>
extern void panic_exit(const char *fmt, ...);

int main() {
    char buf[] = "Hello";
    die("Failed assertion.");
    print(5, buf);
    return 0;
}
