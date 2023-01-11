#include "tty.h"
#include "gdt.h"
#include "idt.h"
#include "shell.h"
#include "paging.h"

void kmain(void)
{
	gdt_init();
	idt_init();
	kinit();

	banner();

	paging_init();
	//asm volatile ("int $0x3");
	//asm volatile ("int $0x4");

	//int *ptr = (int*)0xa0000000;
	//printk("%d", *ptr);

	//shell();
}
