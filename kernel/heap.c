#include "heap.h"
#include "kheap.h"
#include "paging.h"

extern page_dir_t *kernel_dir;

static int32_t find_smallest_hole(uint32_t size, uint8_t page_align, heap_t *heap)
{
	uint32_t i;

	for (i = 0; i < heap->index.size; ++i) {
		header_t *header = (header_t*)lookup_ordered_array(i, &heap->index);
		if (page_align > 0) {
			uint32_t location = (uint32_t)header;
			int32_t offset = 0;
			if ((location + sizeof(header_t)) & 0xfffff000)
				offset = PAGE_SIZE - (location + sizeof(header_t)) % PAGE_SIZE;
			int32_t hole_size = (int32_t)header->size - offset;
			if (hole_size >= (int32_t)size)
				break;
		} else if (header->size >= size) {
			break;
		}
	}

	return i == heap->index.size ? -1 : (int32_t)i;
}

static int8_t header_t_less_than(void *a, void *b)
{
	return ((header_t*)a)->size < ((header_t*)b)->size; // ? 1 : 0;
}

heap_t *create_heap(uint32_t start, uint32_t end,
		uint32_t max, uint8_t supervisor, uint8_t readonly)
{
	if (!IS_ALIGNED(start) || !IS_ALIGNED(end))
		return (void*)0;

	heap_t *heap = (heap_t*)kmalloc(sizeof(heap_t));

	heap->index = place_ordered_array((void*)start, HEAP_INDEX_SIZE, &header_t_less_than);

	start += sizeof(type_t) * HEAP_INDEX_SIZE;
	if (!IS_ALIGNED(start)) {
		start &= 0xfffff000;
		start += PAGE_SIZE;
	}

	heap->start_addr = start;
	heap->end_addr = end;
	heap->max_addr = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;

	header_t *hole = (header_t*)start;
	hole->size = end - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_ordered_array((void*)hole, &heap->index);

	return heap;
}

static void expand(uint32_t new_size, heap_t *heap)
{
	if (new_size & 0xfffff000) {
		new_size &= 0xfffff000;
		new_size += PAGE_SIZE;
	}

	for (uint32_t i = heap->end_addr - heap->start_addr; i < new_size; i += PAGE_SIZE)
		alloc_frame(get_page(heap->start_addr+i, 1, kernel_dir),
				!!heap->supervisor, !heap->readonly);

	heap->end_addr = heap->start_addr + new_size;
}

static uint32_t shrink(uint32_t new_size, heap_t *heap)
{
	if (new_size & PAGE_SIZE) {
		new_size &= PAGE_SIZE;
		new_size += PAGE_SIZE;
	}
	if (new_size < HEAP_MIN_SIZE)
		new_size = HEAP_MIN_SIZE;

	for (uint32_t i = heap->end_addr - heap->start_addr; new_size < i; i -= PAGE_SIZE)
		free_frame(get_page(heap->start_addr+i, 0, kernel_dir));

	heap->end_addr = heap->start_addr + new_size;

	return new_size;
}

void *alloc(uint32_t size, uint8_t page_align, heap_t *heap)
{
	uint32_t new_size = size + sizeof(header_t) + sizeof(footer_t);
	int32_t i = find_smallest_hole(new_size, page_align, heap);

	if (i == -1) {
		uint32_t old_length = heap->end_addr - heap->start_addr;
		uint32_t old_end_addr = heap->end_addr;

		expand(old_length + new_size, heap);
		uint32_t new_length = heap->end_addr - heap->start_addr;

		uint32_t idx = (uint32_t)-1, value = 0;
		for (i = 0; (uint32_t)i < heap->index.size; ++i) {
			uint32_t tmp = (uint32_t)lookup_ordered_array(i, &heap->index);
			if (tmp > value) {
				value = tmp;
				idx = i;
			}
		}

		if (idx == (uint32_t)-1) {
			header_t *header = (header_t*)old_end_addr;
			header->magic = HEAP_MAGIC;
			header->size = new_length - old_length;
			header->is_hole = 1;
			footer_t *footer = (footer_t*)(old_end_addr + header->size - sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
			insert_ordered_array((void*)header, &heap->index);
		} else {
			header_t *header = lookup_ordered_array(idx, &heap->index);
			header->size += new_length - old_length;
			footer_t *footer = (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}

		return alloc(size, page_align, heap);
	}

	header_t *orig_hole_header = (header_t*)lookup_ordered_array(i, &heap->index);
	uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
	uint32_t orig_hole_size = orig_hole_header->size;
	if (orig_hole_size-new_size < sizeof(header_t) + sizeof(footer_t)) {
		size += orig_hole_size-new_size;
		new_size = orig_hole_size;
	}
	if (page_align && (orig_hole_pos & 0xfffff000)) {
		uint32_t new_location = orig_hole_pos + PAGE_SIZE - (orig_hole_pos & 0xfff) - sizeof(header_t);
		header_t *hole_header = (header_t *)orig_hole_pos;
		hole_header->size = PAGE_SIZE - (orig_hole_pos & 0xfff) - sizeof(header_t);
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		footer_t *hole_footer = (footer_t*)((uint32_t)new_location - sizeof(footer_t));
		hole_footer->magic = HEAP_MAGIC;
		hole_footer->header = hole_header;
		orig_hole_pos = new_location;
		orig_hole_size = orig_hole_size - hole_header->size;
	} else {
		remove_ordered_array(i, &heap->index);
	}

	header_t *block_header = (header_t*)orig_hole_pos;
	block_header->magic = HEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size = new_size;

	footer_t *block_footer = (footer_t*)(orig_hole_pos + sizeof(header_t) + size);
	block_footer->magic = HEAP_MAGIC;
	block_footer->header = block_header;

	if (orig_hole_size - new_size > 0) {
		header_t *hole_header = (header_t*)(orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		hole_header->size = orig_hole_size - new_size;
		footer_t *hole_footer = (footer_t*)((uint32_t)hole_header + orig_hole_size - new_size - sizeof(footer_t));
		if ((uint32_t)hole_footer < heap->end_addr) {
			hole_footer->magic = HEAP_MAGIC;
			hole_footer->header = hole_header;
		}
		insert_ordered_array((void*)hole_header, &heap->index);
	}

	return (void*)((uint32_t)block_header+sizeof(header_t));
}

void free(void *p, heap_t *heap)
{
	if (p == 0)
		return;

	header_t *header = (header_t*)((uint32_t)p - sizeof(header_t));
	footer_t *footer = (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));

	header->is_hole = 1;

	char do_add = 1;

	footer_t *test_footer = (footer_t*)((uint32_t)header - sizeof(footer_t));
	if (test_footer->magic == HEAP_MAGIC && test_footer->header->is_hole == 1) {
		uint32_t cache_size = header->size;
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		do_add = 0;
	}

	header_t *test_header = (header_t*)((uint32_t)footer + sizeof(footer_t));
	if (test_header->magic == HEAP_MAGIC && test_header->is_hole) {
		header->size += test_header->size;
		test_footer = (footer_t*)((uint32_t)test_header + test_header->size - sizeof(footer_t));
		footer = test_footer;
		uint32_t it = 0;
		while ((it < heap->index.size) &&
				(lookup_ordered_array(it, &heap->index) != (void*)test_header))
			it++;
		remove_ordered_array(it, &heap->index);
	}

	if ((uint32_t)footer+sizeof(footer_t) == heap->end_addr) {
		uint32_t old_length = heap->end_addr - heap->start_addr;
		uint32_t new_length = shrink((uint32_t)header - heap->start_addr, heap);
		if (header->size - (old_length-new_length) > 0) {
			header->size -= old_length-new_length;
			footer = (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
		} else {
			uint32_t it = 0;
			while ((it < heap->index.size) &&
					(lookup_ordered_array(it, &heap->index) != (void*)test_header))
				it++;
			if (it < heap->index.size)
				remove_ordered_array(it, &heap->index);
		}
	}

	if (do_add == 1)
		insert_ordered_array((void*)header, &heap->index);
}
