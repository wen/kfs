#ifndef STACK_H
#define STACK_H
#include <stddef.h>

#define GET_EBP(x) asm volatile("mov %%ebp, %0" : "=r"(x) ::)
#define GET_ESP(x) asm volatile("mov %%esp, %0" : "=r"(x) ::)

void print_memory(const void *addr, size_t size);

#endif
