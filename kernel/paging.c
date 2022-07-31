#include "paging.h"
#include "kheap.h"
#include "heap.h"
#include "string.h"
#include "panic.h"
#include "tty.h"

extern uint32_t placement_addr;
extern heap_t *kheap;
extern void paging_flush(uint32_t);

uint32_t *frames;
uint32_t nframes;
page_dir_t *kernel_dir;
page_dir_t *current_dir;

static void set_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / PAGE_SIZE;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / PAGE_SIZE;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

static uint32_t first_frame(void)
{
	for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
		if (frames[i] != 0xffffffff) {
			for (uint32_t j = 0; j < 32; ++j) {
				if (!(frames[i] & (0x1 << j)))
					return i * 32 + j;
			}
		}
	}
	return 0;
}

void alloc_frame(page_t *page, int is_kernel, int is_writable)
{
	if (page->frame)
		return;
	uint32_t idx = first_frame();
	if (idx == (uint32_t)-1)
		panic("no free frames");
	set_frame(idx * PAGE_SIZE);
	page->present = 1;
	page->rw = !!is_writable;
	page->user = !is_kernel;
	page->frame = idx;
}

void free_frame(page_t *page)
{
	uint32_t frame = page->frame;

	if (!frame)
		return;
	clear_frame(frame);
	page->frame = 0x0;
}

page_t *get_page(uint32_t addr, int make, page_dir_t *dir)
{
	addr /= PAGE_SIZE;
	uint32_t index = addr / 1024;
	if (dir->tables[index])
		return &dir->tables[index]->pages[addr%1024];
	if (make) {
		uint32_t tmp;
		dir->tables[index] = (page_tab_t*)kmalloc_ap(sizeof(page_tab_t), &tmp);
		dir->tables_phys[index] = tmp | 0x7;
		return &dir->tables[index]->pages[addr%1024];
	}

	return (void*)0;
}

void paging_init(void)
{
	nframes = 0x1000000 / PAGE_SIZE;
	frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
	bzero(frames, INDEX_FROM_BIT(nframes));

	kernel_dir = (page_dir_t*)kmalloc_a(sizeof(page_dir_t));
	bzero(kernel_dir, sizeof(page_dir_t));
	current_dir = kernel_dir;

	for (uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
		get_page(i, 1, kernel_dir);

	for (uint32_t i = 0; i < placement_addr + PAGE_SIZE; i += PAGE_SIZE)
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);

	for (uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);

	paging_flush((uint32_t)kernel_dir->tables_phys);
	kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xcffff000, 0, 0);
}
