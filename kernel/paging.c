#include "paging.h"
#include "kheap.h"
#include "heap.h"
#include "string.h"
#include "panic.h"
#include "tty.h"
#include "isr.h"

extern uintptr_t placement_addr;
extern heap_t *kheap;
extern void paging_flush(uintptr_t);

uint32_t *frames;
uint32_t nframes;
page_dir_t *kernel_dir;

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
	panic("no free frames");
	return -1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writable)
{
	if (page->frame)
		return;
	uint32_t idx = first_frame();
	set_frame(idx * PAGE_SIZE);
	page->present = 1;
	page->writable = !!is_writable;
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
		dir->tables[index] = malloc_ap(sizeof(page_tab_t), &tmp);
		bzero(dir->tables[index], PAGE_SIZE);
		dir->tables_phys[index] = tmp | 0x7;
		return &dir->tables[index]->pages[addr%1024];
	}

	return NULL;
}

void page_fault(registers_t regs)
{
	uintptr_t faulting_addr;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_addr));

	int preset = !(regs.err_code & 0x1);
	int writable = regs.err_code & 0x2;
	int user = regs.err_code & 0x4;
	int reserved = regs.err_code & 0x8;

	printk("Page fault! ( ");
	if (preset)
		printk("present ");
	if (writable)
		printk("read-only ");
	if (user)
		printk("user-mode ");
	if (reserved)
		printk("reserved ");
	printk(") at 0x%08x\n", faulting_addr);

	for(;;);
}

void paging_init(void)
{
	nframes = MEM_SIZE / PAGE_SIZE;
	frames = malloc(INDEX_FROM_BIT(nframes));
	bzero(frames, INDEX_FROM_BIT(nframes));

	kernel_dir = malloc_a(sizeof(page_dir_t));
	bzero(kernel_dir, sizeof(page_dir_t));

	for (uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += PAGE_SIZE)
		get_page(i, 1, kernel_dir);

	for (uint32_t i = 0; i < placement_addr + PAGE_SIZE; i += PAGE_SIZE)
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);

	for (uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += PAGE_SIZE)
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);

	register_interrupt_handler(14, page_fault);

	paging_flush((uintptr_t)kernel_dir->tables_phys);

	kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INIT_SIZE, KHEAP_END, 0, 0);
}

uintptr_t get_physical_addr(uintptr_t addr)
{
	if (!addr)
		return 0;

	uint32_t offset = addr % PAGE_SIZE;

	page_t *page = get_page(addr, 0, kernel_dir);

	if (!page)
		return 0;

	return page->frame * PAGE_SIZE + offset;
}

uintptr_t get_virtual_addr(uintptr_t addr)
{
	if (!addr)
		return 0;

	uint32_t offset = addr % PAGE_SIZE;
	addr /= PAGE_SIZE;
	uint32_t dir_i, page_i;

	for (dir_i = 0; dir_i != 1024; ++dir_i) {
		if (kernel_dir->tables[dir_i]) {
			page_tab_t *page_tab = kernel_dir->tables[dir_i];
			for (page_i = 0; page_i != 1024; ++page_i) {
				if (page_tab->pages[page_i].frame == addr) {
					return (dir_i*1024 + page_i)*PAGE_SIZE+offset;
				}
			}
		}
	}
	return 0;
}
