#include "tty.h"
#include "gdt.h"
#include "shell.h"

void kmain(void)
{
	kinit();

	banner();

	gdt_init();

	shell();
}
