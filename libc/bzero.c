#include "string.h"

void bzero(void *s, size_t n)
{
	unsigned char *ptr = s;

	while (n-- != 0)
		*ptr++ = '\0';
}
