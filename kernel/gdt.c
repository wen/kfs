#include "gdt.h"
#include "tty.h"

void create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
	uint64_t descriptor;

	descriptor  =  limit       & 0x000F0000;
	descriptor |= (flag <<  8) & 0x00F0FF00;
	descriptor |= (base >> 16) & 0x000000FF;
	descriptor |=  base        & 0xFF000000;

	descriptor <<= 32;

	descriptor |= base << 16;
	descriptor |= limit & 0x0000FFFF;

	printk("0x%.16llX\n", descriptor);
}

void gdt_init(void)
{
	create_descriptor(0, 0, 0);
	create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
	create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
	create_descriptor(0, 0x000FFFFF, (GDT_STACK_PL0));
	create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL3));
	create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL3));
	create_descriptor(0, 0x000FFFFF, (GDT_STACK_PL3));
}
