#include "tty.h"
#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "kheap.h"
#include "heap.h"
#include "string.h"

void test(void)
{
	void **ptr = malloc(0x400 * sizeof(void*));

	for (int i = 0; i != 0x400; ++i) {
		ptr[i] = malloc(8);
		memcpy(ptr[i], "AAAAAAAA", 8);
	}

	printk("0x%08x - size: %u\n", ptr[0xf0], get_size(ptr[0xf0]));
	free(ptr[0xf0]);
	ptr[0xf0] = NULL;
	free(ptr[0xf1]);
	ptr[0xf1] = NULL;
	free(ptr[0xf2]);
	ptr[0xf2] = NULL;
	free(ptr[0xf3]);
	ptr[0xf3] = NULL;

	void *new = malloc(32);
	printk("0x%08x - size: %u\n", new, get_size(new));
	free(new);

	for (int i = 0; i != 0x400; ++i)
		free(ptr[i]);
}

void kmain(void)
{
	gdt_init();
	idt_init();
	paging_init();
	kinit();
	banner();
	test();
}
