#include "string.h"

void *memcpy(void *dst, const void *src, size_t n)
{
	unsigned char *dstp = dst;
	const unsigned char *srcp = src;

	while (n-- != 0)
		*dstp++ = *srcp++;

	return dst;
}
