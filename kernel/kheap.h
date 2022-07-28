#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>

#define KHEAP_START 0xc0000000
#define KHEAP_INITIAL_SIZE 0x100000

uint32_t kmalloc_int(uint32_t sz, int align, uint32_t *phys);
uint32_t kmalloc_a(uint32_t sz);
uint32_t kmalloc_p(uint32_t sz, uint32_t *phys);
uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys);
uint32_t kmalloc(uint32_t sz);
void kfree(void *p);

#endif
