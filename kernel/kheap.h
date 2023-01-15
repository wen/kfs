#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include <stdint.h>

#define IS_ALIGNED(x)		(!((x) & 0x00000fff))
#define ALIGN(x)			(((x) + 0x00000fff) & 0xfffff000)
#define KHEAP_START 		0xc0000000
#define KHEAP_END 			0xc01ff000
#define KHEAP_INITIAL_SIZE	0x100000

void *kmalloc_a(size_t sz);
void *kmalloc_p(size_t sz, uintptr_t *phys);
void *kmalloc_ap(size_t sz, uintptr_t *phys);
void *kmalloc(size_t sz);
void kfree(void *p);

#endif
