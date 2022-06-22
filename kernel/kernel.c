#include "tty.h"
#include "gdt.h"
#include "shell.h"

void kernel_main(void)
{
	kinit();

	banner();

	gdt_init();

	shell();
}
