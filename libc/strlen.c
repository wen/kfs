#include "string.h"

size_t strlen(const char *str)
{
	const char *save = str;

	while (*str != '\0')
		++str;

	return str - save;
}
