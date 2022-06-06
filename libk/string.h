#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#define TODIGIT(c)	((c) - '0')
#define ISDIGIT(c)	((unsigned)FT_TODIGIT(c) <= 9)
#define TOCHAR(n)	((n) + '0')

void *memset(void *b, int c, size_t len);
void *memcpy(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char *s);

#endif
