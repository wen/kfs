#include "tty.h"
#include "gdt.h"
#include "shell.h"
#include "paging.h"

void kmain(void)
{
	kinit();

	gdt_init();

	paging_init();

	banner();

	shell();
}
