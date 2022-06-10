#include "string.h"
#include "printflocal.h"

int printk(const char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = 0;
	va_end(ap);

	return ret;
}
