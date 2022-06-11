#include "string.h"

char *strchr(const char *s, int c)
{
	char ch = c;

	while (*s != ch) {
		if (*s == '\0')
			return (NULL);
		++s;
	}

	return (char *)s;
}
