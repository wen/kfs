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

	// create a hole and show its address
	printk("0x%08x - size: %u\n", ptr[0xf0], size(ptr[0xf0]));
	free(ptr[0xf0]);
	ptr[0xf0] = NULL;
	free(ptr[0xf1]);
	ptr[0xf1] = NULL;
	free(ptr[0xf2]);
	ptr[0xf2] = NULL;
	free(ptr[0xf3]);
	ptr[0xf3] = NULL;

	// allocate something to fill the hole
	void *new = malloc(32);
	printk("0x%08x - size: %u\n", new, size(new));
	free(new);

	for (int i = 0; i != 0x400; ++i)
		free(ptr[i]);
	free(ptr);

	void *kptr = kmalloc(1024);
	printk("kptr: 0x%08x - size: %u\n", kptr, ksize(kptr));
	kfree(kptr);
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
