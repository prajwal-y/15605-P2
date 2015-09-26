/** @file print_test.c
 *  @brief Test the print function call
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscall.h>
#include <simics.h>

int main() {
    char buf[] = "Hello";
    print(10, buf);
    return 0;
}
