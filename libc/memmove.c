#include "string.h"

void *memmove(void *dst, const void *src, size_t len)
{
	unsigned char *dstp;
	const unsigned char *srcp;

	if (dst == src || len == 0)
		return dst;
	dstp = dst;
	srcp = src;
	if (dst < src) {
		while (len-- != 0)
			*dstp++ = *srcp++;
	} else {
		dstp += len - 1;
		srcp += len - 1;
		while (len-- != 0)
			*dstp-- = *srcp--;
	}

	return dst;
}
