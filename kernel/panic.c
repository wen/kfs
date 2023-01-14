#include "panic.h"
#include "tty.h"
#include "io.h"

void panic(const char *msg)
{
	asm volatile("cli");

	printk("KFS PANIC at %s:%d - %s\n", __FILE__, __LINE__, msg);

	for(;;);
	//shutdown();
}
