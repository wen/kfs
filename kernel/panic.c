#include "panic.h"
#include "tty.h"

void panic(const char *msg)
{
	printk("KFS PANIC at %s:%d - %s\n", __FILE__, __LINE__, msg);
}
