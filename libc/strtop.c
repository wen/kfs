#include "string.h"

uintptr_t strtop(const char *str)
{
	uintptr_t retval = 0;

	while (*str) {
		uint8_t byte = *str++;
		if (byte >= '0' && byte <= '9')
			byte -= '0';
		else if (byte >= 'a' && byte <= 'f')
			byte -= 'a' + 10;
		else if (byte >= 'A' && byte <= 'F')
			byte -= 'A' + 10;
		retval = (retval << 4) | (byte & 0xf);
	}

	return retval;
}
