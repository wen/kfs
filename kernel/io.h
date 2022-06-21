#ifndef IO_H
#define IO_H
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;

	asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );

	return ret;
}

static inline void io_wait(void)
{
	outb(0x80, 0);
}

#endif
