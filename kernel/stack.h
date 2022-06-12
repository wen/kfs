#ifndef STACK_H
#define STACK_H

static inline int get_esp(void)
{
	int ret;

	asm volatile("mov %%esp, %0" : "=r"(ret));

	return ret;
}

static inline int get_ebp(void)
{
	int ret;

	asm volatile("mov %%ebp, %0" : "=r"(ret));

	return ret;
}

#endif
