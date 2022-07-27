#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include "ordered_array.h"

#define HEAP_INDEX_SIZE	0x20000
#define HEAP_MAGIC		0x0000002A
#define HEAP_MIN_SIZE	0x70000

typedef struct header_s {
	uint32_t magic;
	uint8_t is_hole;
	uint32_t size;
} header_t;

typedef struct footer_s {
	uint32_t magic;
	header_t *header;
} footer_t;

typedef struct heap_s {
	ordered_array_t index;
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t max_addr;
	uint8_t supervisor;
	uint8_t readonly;
} heap_t;

heap_t *create_heap(uint32_t, uint32_t, uint32_t, uint8_t, uint8_t);
void *alloc(uint32_t size, uint8_t page_align, heap_t *heap);
void free(void *p, heap_t *heap);

#endif
