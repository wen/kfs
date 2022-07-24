#include "paging.h"
#include "heap.h"

extern uint32_t placement_addr;

uint32_t *frames;
uint32_t nframes;

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

static uint32_t test_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);

	return frames[idx] & (0x1 << off);
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
}
