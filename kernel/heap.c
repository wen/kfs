#include "heap.h"
#include "kheap.h"
#include "paging.h"
#include "panic.h"

extern page_dir_t *kernel_dir;

static int find_smallest_hole(size_t size, uint8_t page_align, heap_t *heap)
{
	size_t i;

	for (i = 0; i < heap->index.size; ++i) {
		header_t *header = lookup_vec(i, &heap->index);
		if (page_align > 0) {
			uintptr_t location = (uintptr_t)header;
			int offset = 0;
			if (!IS_ALIGNED(location + sizeof(header_t)))
				offset = PAGE_SIZE - (location + sizeof(header_t)) % PAGE_SIZE;
			int hole_size = (int)header->size - offset;
			if (hole_size >= (int)size)
				break;
		} else if (header->size >= size) {
			break;
		}
	}

	return i == heap->index.size ? -1 : (int)i;
}

static int header_t_less_than(void *a, void *b)
{
	return ((header_t*)a)->size < ((header_t*)b)->size;
}

heap_t *create_heap(uintptr_t start, uintptr_t end,
		uintptr_t max, uint8_t supervisor, uint8_t readonly)
{
	if (!IS_ALIGNED(start) || !IS_ALIGNED(end))
		panic("heap error: address not aligned!");

	heap_t *heap = kmalloc(sizeof(heap_t));

	heap->index = create_vec((void*)start, HEAP_INDEX_SIZE, &header_t_less_than);

	start += sizeof(void*) * HEAP_INDEX_SIZE;
	if (!IS_ALIGNED(start))
		start = ALIGN(start);

	heap->start_addr = start;
	heap->end_addr = end;
	heap->max_addr = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;

	header_t *hole = (header_t*)start;
	hole->size = end - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_vec(hole, &heap->index);

	return heap;
}

static void expand(size_t new_size, heap_t *heap)
{
	if (new_size <= heap->end_addr - heap->start_addr)
		panic("unable to expand the heap");

	if (!IS_ALIGNED(new_size))
		new_size = ALIGN(new_size);

	if (new_size + heap->start_addr > heap->max_addr)
		panic("no more space");

	uintptr_t i = heap->end_addr - heap->start_addr;
	while (i < new_size) {
		alloc_frame(get_page(heap->start_addr+i, 1, kernel_dir),
					!!heap->supervisor, !heap->readonly);
		i += PAGE_SIZE;
	}

	heap->end_addr = heap->start_addr + new_size;
}

static size_t shrink(size_t new_size, heap_t *heap)
{
	if (new_size > heap->end_addr - heap->start_addr)
		panic("unable to shrink the heap");

	if (!IS_ALIGNED(new_size))
		new_size = ALIGN(new_size);
	if (new_size < HEAP_MIN_SIZE)
		new_size = HEAP_MIN_SIZE;

	uintptr_t i = heap->end_addr - heap->start_addr;
	while (new_size < i) {
		free_frame(get_page(heap->start_addr+i, 0, kernel_dir));
		i -= PAGE_SIZE;
	}

	heap->end_addr = heap->start_addr + new_size;

	return new_size;
}

void *heap_alloc(size_t size, uint8_t page_align, heap_t *heap)
{
	size_t new_size = size + sizeof(header_t) + sizeof(footer_t);

	// try to find empty space
	int i = find_smallest_hole(new_size, page_align, heap);

	// if not found then expand
	if (i == -1) {
		uintptr_t old_length = heap->end_addr - heap->start_addr;
		uintptr_t old_end_addr = heap->end_addr;

		expand(old_length + new_size, heap);
		uintptr_t new_length = heap->end_addr - heap->start_addr;

		uint32_t idx = (uint32_t)-1, value = 0;
		for (i = 0; (uint32_t)i < heap->index.size; ++i) {
			uintptr_t tmp = (uintptr_t)lookup_vec(i, &heap->index);
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
			insert_vec((void*)header, &heap->index);
		} else {
			header_t *header = lookup_vec(idx, &heap->index);
			header->size += new_length - old_length;
			footer_t *footer = (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}

		return heap_alloc(size, page_align, heap);
	}

	header_t *orig_hole_header = (header_t*)lookup_vec(i, &heap->index);
	uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
	uint32_t orig_hole_size = orig_hole_header->size;
	if (orig_hole_size-new_size < sizeof(header_t) + sizeof(footer_t)) {
		size += orig_hole_size-new_size;
		new_size = orig_hole_size;
	}
	if (page_align && !IS_ALIGNED(orig_hole_pos)) {
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
		remove_vec(i, &heap->index);
	}

	// change header and footer
	header_t *block_header = (header_t*)orig_hole_pos;
	block_header->magic = HEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size = new_size;

	footer_t *block_footer = (footer_t*)(orig_hole_pos + sizeof(header_t) + size);
	block_footer->magic = HEAP_MAGIC;
	block_footer->header = block_header;

	// create a new hole after the allocation
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
		insert_vec((void*)hole_header, &heap->index);
	}

	return (void*)((uint32_t)block_header+sizeof(header_t));
}

void heap_free(void *p, heap_t *heap)
{
	if (p == 0)
		return;

	header_t *header = (header_t*)((uint32_t)p - sizeof(header_t));
	footer_t *footer = (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));

	if (header->magic != HEAP_MAGIC || footer->magic != HEAP_MAGIC)
		panic("heap error: memory corrupted");

	header->is_hole = 1;

	int valid = 1;

	// try to merge left
	footer_t *test_footer = (footer_t*)((uint32_t)header - sizeof(footer_t));
	if (test_footer->magic == HEAP_MAGIC && test_footer->header->is_hole == 1) {
		uint32_t cache_size = header->size;
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		valid = 0;
	}

	// try to merge right
	header_t *test_header = (header_t*)((uint32_t)footer + sizeof(footer_t));
	if (test_header->magic == HEAP_MAGIC && test_header->is_hole) {
		header->size += test_header->size;
		test_footer = (footer_t*)((uint32_t)test_header + test_header->size - sizeof(footer_t));
		footer = test_footer;
		uint32_t it = 0;
		while ((it < heap->index.size) &&
				(lookup_vec(it, &heap->index) != (void*)test_header))
			it++;

		if (it >= heap->index.size)
			panic("heap error");
		remove_vec(it, &heap->index);
	}

	// we can contract if we hit end
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
					(lookup_vec(it, &heap->index) != (void*)test_header))
				it++;
			if (it < heap->index.size)
				remove_vec(it, &heap->index);
		}
	}

	if (valid == 1)
		insert_vec(header, &heap->index);
}

size_t get_size(void *ptr)
{
	header_t *header = (header_t*)((uintptr_t)ptr - sizeof(header_t));

	if (header->magic != HEAP_MAGIC)
		panic("heap error: memory corrupted");

	return header->size - sizeof(header_t) - sizeof(footer_t);
}
