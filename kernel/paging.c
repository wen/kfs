#include "paging.h"
#include "heap.h"
#include "string.h"
#include "isr.h"

extern uint32_t placement_addr;
extern void paging_flush(uint32_t);

uint32_t *frames;
uint32_t nframes;
page_dir_t *kernel_dir = 0;
page_dir_t *current_dir = 0;

static void set_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

/*
static uint32_t test_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);

	return frames[idx] & (0x1 << off);
}
*/

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
	{} // PANIC
	set_frame(idx * 0x1000);
	page->present = 1;
	page->rw = !!is_writable;
	page->user = !is_kernel;
	//page->rw = is_writable ? 1 : 0;
	//page->user = !is_kernel ? 0 : 1;
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

/*
void switch_page_dir(page_dir_t *dir)
{
	uint32_t cr0;

	current_dir = dir;
	asm volatile("mov %0, %%cr3":: "r"(&dir->tables_phys));
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}
*/

page_t *get_page(uint32_t addr, int make, page_dir_t *dir)
{
	addr /= 0x1000;
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

void page_fault(registers_t regs)
{
	(void)regs;
}

void paging_init(void)
{
	uint32_t mem_end_page = 0x1000000;

	nframes = mem_end_page / 0x1000;
	frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
	bzero(frames, INDEX_FROM_BIT(nframes));

	kernel_dir = (page_dir_t*)kmalloc_a(sizeof(page_dir_t));
	bzero(kernel_dir, sizeof(page_dir_t));
	current_dir = kernel_dir;

	for (uint32_t i = 0; i < placement_addr; i += PAGE_SIZE)
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
	register_interrupt_handler(14, page_fault);
	paging_flush((uint32_t)kernel_dir->tables_phys);
	//switch_page_dir(kernel_dir);
}
