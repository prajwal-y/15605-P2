/** @file x86/asm.h
 *  @brief x86-specific assembly functions
 *  @author matthewj S2008
 */

#ifndef X86_ASM_H
#define X86_ASM_H

#include <stdint.h>

/** @brief Atomically test the value of a memory location and set to 0. */
int test_and_unset(void *target);

/** @brief Atomically test the value of a memory location and set to 1. */
int test_and_set(void *target);

#endif /* !X86_ASM_H */
