#include <limits.h>
#include "string.h"

long strtol(const char *nptr, char **endptr, int base)
{
	const char *s;
	unsigned long acc;
	char c;
	unsigned long cutoff;
	int neg, any, cutlim;

	s = nptr;
	do {
		c = *s++;
	} while (ISSPACE((unsigned char)c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}
	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X') &&
		((s[1] >= '0' && s[1] <= '9') ||
		(s[1] >= 'A' && s[1] <= 'F') ||
		(s[1] >= 'a' && s[1] <= 'f'))) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	acc = any = 0;
	if (base < 2 || base > 36)
		goto noconv;

	cutoff = neg ? (unsigned long)-(LONG_MIN + LONG_MAX) + LONG_MAX : LONG_MAX;
	cutlim = cutoff % base;
	cutoff /= base;
	for (; ; c = *s++) {
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0)
		acc = neg ? LONG_MIN : LONG_MAX;
	else if (neg)
		acc = -acc;
noconv:
	if (endptr != NULL)
		*endptr = (char *)(any ? s - 1 : nptr);

	return acc;
}
