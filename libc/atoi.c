#include "string.h"

int atoi(const char *str)
{
	int ret = 0;
	int sign = 1;

	while (ISSPACE(*str))
		++str;
	if (*str == '-' || *str == '+')
		sign = *str++ == '-' ? -1 : 1;
	while (ISDIGIT(*str))
		ret = ret * 10 + TODIGIT(*str++);

	return (ret * sign);
}
