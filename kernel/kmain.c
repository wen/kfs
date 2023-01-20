#include "tty.h"
#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "kheap.h"
#include "heap.h"
#include "shell.h"
#include "string.h"

void kmain(void)
{
	gdt_init();
	idt_init();
	paging_init();
	kinit();
	banner();

	void *a = malloc(16);
	printk("a: 0x%08x, size: %u\n", a, get_size(a));
	void *b = malloc(16);
	printk("b: 0x%08x, size: %u\n", b, get_size(b));

	free(a);
	free(b);
	void *c = malloc(32);
	printk("c: 0x%08x, size: %u\n", c, get_size(c));
}
