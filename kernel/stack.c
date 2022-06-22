#include <stdint.h>
#include "stack.h"
#include "string.h"
#include "tty.h"

static void putstr_hex(const unsigned char *str, size_t size)
{
	while (size-- != 0) {
		if (ISPRINT(*str))
			kputchar(*str);
		else
			kputchar('.');
		++str;
	}
	kputchar('\n');
}

void print_memory(const void *addr, size_t size)
{
	const unsigned char *ptr = addr;

	for (size_t i = 0; i < size; ++i, ++ptr) {
		if (i % 16 == 0)
			printk("0x%08X  ", ptr);
		printk("%02hhx", *ptr);
		if ((i & 0x1) != 0)
			kputchar(' ');
		if ((i + 1) % 16 == 0)
			putstr_hex(ptr - 15, 16);
		if (i == size - 1 && (i + 1) % 16 != 0) {
			while ((++i + 1) % 16 != 0) {
				kputstr("  ");
				if ((i & 0x1) != 0)
					kputchar(' ');
			}
			kputstr("   ");
			putstr_hex(ptr - size % 16 + 1, size % 16);
		}
	}
}
