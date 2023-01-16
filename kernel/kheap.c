#include "kheap.h"
#include "paging.h"
#include "heap.h"

extern uintptr_t end;
uintptr_t placement_addr = (uintptr_t)&end;
extern page_dir_t *kernel_dir;
heap_t *kheap;

static void *kmalloc_int(size_t sz, uint8_t align, uintptr_t *phys)
{
	if (kheap) {
		void *addr = alloc(sz, align, kheap);
		if (phys) {
			page_t *page = get_page((uint32_t)addr, 0, kernel_dir);
			*phys = page->frame * PAGE_SIZE + ((uint32_t)addr & 0xfff);
		}
		return addr;
	}

	if (align == 1 && !IS_ALIGNED(placement_addr))
		placement_addr = ALIGN(placement_addr);

	if (phys)
		*phys = placement_addr;
	void *tmp = (void*)placement_addr;
	placement_addr += sz;
	return tmp;
}

void kfree(void *p)
{
	free(p, kheap);
}

void *kmalloc_a(size_t sz)
{
	return kmalloc_int(sz, 1, 0);
}

void *kmalloc_p(size_t sz, uintptr_t *phys)
{
	return kmalloc_int(sz, 0, phys);
}

void *kmalloc_ap(size_t sz, uintptr_t *phys)
{
	return kmalloc_int(sz, 1, phys);
}

void *kmalloc(size_t sz)
{
	return kmalloc_int(sz, 0, 0);
}

void *kbrk(size_t sz)
{
	if (sz == 0)
		return NULL;

	void *ret = (void*)placement_addr;
	placement_addr += sz;
	return ret;
}
