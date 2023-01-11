#include "tty.h"
#include "gdt.h"
#include "shell.h"
#include "paging.h"

void kmain(void)
{
	gdt_init();

	kinit();

	banner();

	paging_init();

	//shell();
}
