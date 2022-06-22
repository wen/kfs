#include "gdt.h"
#include "tty.h"

#include "stack.h"

extern void gdt_flush(uintptr_t);

uint64_t gdt_arr[7];
gdt_ptr_t *gdt_ptr = (gdt_ptr_t*)GDT_PTR;

static uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
	uint64_t descriptor;

	descriptor  =  limit       & 0x000F0000;
	descriptor |= (flag <<  8) & 0x00F0FF00;
	descriptor |= (base >> 16) & 0x000000FF;
	descriptor |=  base        & 0xFF000000;

	descriptor <<= 32;

	descriptor |= base << 16;
	descriptor |= limit & 0x0000FFFF;

	return descriptor;
}

void gdt_init(void)
{
	gdt_ptr->limit = (sizeof(uint64_t) * 7) - 1;
	gdt_ptr->base  = (uintptr_t)&gdt_arr;

	gdt_arr[0] = create_descriptor(0, 0, 0);
	gdt_arr[1] = create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
	gdt_arr[2] = create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
	gdt_arr[3] = create_descriptor(0, 0x000FFFFF, (GDT_STACK_PL0));
	gdt_arr[4] = create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL3));
	gdt_arr[5] = create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL3));
	gdt_arr[6] = create_descriptor(0, 0x000FFFFF, (GDT_STACK_PL3));

	gdt_flush(GDT_PTR);
}
