#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_SIZE 0x1000
#define INDEX_FROM_BIT(x) (x/32)
#define OFFSET_FROM_BIT(x) (x%32)

typedef struct page_s {
	uint32_t present	: 1;
	uint32_t rw			: 1;
	uint32_t user		: 1;
	uint32_t accessed	: 1;
	uint32_t dirty		: 1;
	uint32_t unused		: 7;
	uint32_t frame		: 20;
} page_t;

typedef struct page_tab_s {
	page_t pages[1024];
} page_tab_t;

typedef struct page_dir_s {
	page_tab_t *tables[1024];
	uint32_t tables_phys[1024];
	uint32_t phys;
} page_dir_t;

void paging_init(void);
page_t *get_page(uint32_t addr, int make, page_dir_t *dir);
void alloc_frame(page_t *page, int is_kernel, int is_writable);
void free_frame(page_t *page);

#endif
