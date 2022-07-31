#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include <stdint.h>

#define KHEAP_START 0xc0000000
#define KHEAP_INITIAL_SIZE 0x100000

void *kmalloc_int(size_t sz, int align, uintptr_t *phys);
void *kmalloc_a(size_t sz);
void *kmalloc_p(size_t sz, uintptr_t *phys);
void *kmalloc_ap(size_t sz, uintptr_t *phys);
void *kmalloc(size_t sz);
void kfree(void *p);

#endif
