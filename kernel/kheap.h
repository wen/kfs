#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include <stdint.h>

#define IS_ALIGNED(x)		(!((x) & 0x00000fff))
#define ALIGN(x)			(((x) + 0x00000fff) & 0xfffff000)
#define KHEAP_START 		0xc0000000
#define KHEAP_END 			0xc01ff000
#define KHEAP_INIT_SIZE		0x00100000

void *malloc_a(size_t sz);
void *malloc_p(size_t sz, uintptr_t *phys);
void *malloc_ap(size_t sz, uintptr_t *phys);
void *malloc(size_t sz);
void free(void *p);
void *kbrk(size_t sz);

#endif
