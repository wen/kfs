#include "tty.h"
#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "kheap.h"
#include "shell.h"

void kmain(void)
{
	gdt_init();
	idt_init();
	kinit();

	banner();

	void *a = kmalloc(4);
	paging_init();
	void *b = kmalloc(4);
	void *c = kmalloc(4);

	printk("a: 0x%08x\n", a);
	printk("b: 0x%08x\n", b);
	printk("c: 0x%08x\n", c);

	kfree(b);
	kfree(c);
	void *d = kmalloc(8);
	printk("d: 0x%08x\n", d);

	//asm volatile ("int $0x3");
	//asm volatile ("int $0x4");
	//shell();
}
