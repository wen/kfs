#include "kheap.h"
#include "paging.h"
#include "heap.h"

extern uint32_t end;
uint32_t placement_addr = (uint32_t)&end;
extern page_dir_t *kernel_dir;
heap_t *kheap;

uint32_t kmalloc_int(uint32_t sz, int align, uint32_t *phys)
{
	if (kheap) {
		void *addr = alloc(sz, (uint8_t)align, kheap);
		if (phys) {
			page_t *page = get_page((uint32_t)addr, 0, kernel_dir);
			*phys = page->frame * PAGE_SIZE + ((uint32_t)addr & 0xfff);
		}
		return (uint32_t)addr;
	} else {
		if (align == 1 && (placement_addr & 0xfffff000)) {
			placement_addr &= 0xfffff000;
			placement_addr += PAGE_SIZE;
		}
		if (phys)
			*phys = placement_addr;
		uint32_t tmp = placement_addr;
		placement_addr += sz;
		return tmp;
	}
}

void kfree(void *p)
{
	free(p, kheap);
}

uint32_t kmalloc_a(uint32_t sz)
{
	return kmalloc_int(sz, 1, 0);
}

uint32_t kmalloc_p(uint32_t sz, uint32_t *phys)
{
	return kmalloc_int(sz, 0, phys);
}

uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys)
{
	return kmalloc_int(sz, 1, phys);
}

uint32_t kmalloc(uint32_t sz)
{
	return kmalloc_int(sz, 0, 0);
}
