#include "string.h"

size_t strnlen(const char *s, size_t maxlen)
{
	size_t len = 0;

	while (len < maxlen) {
		if (!*s)
			break;
		++len;
		++s;
	}

	return len;
}
