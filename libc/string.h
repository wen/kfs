#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

#define TODIGIT(c)	((c) - '0')
#define ISDIGIT(c)	((unsigned)TODIGIT(c) <= 9)
#define TOCHAR(n)	((n) + '0')
#define ISASCII(c)	((c) >= 0x00 && (c) <= 0x7f)
#define ISPRINT(c)	((c) >= 0x20 && (c) <= 0x7e)
#define ISSPACE(c)	(((c) >= 0x09 && (c) <= 0x0d) || (c) == 0x20)
#define TOLOWER(c)	((c) >= 'A' && (c) <= 'Z' ? (c) ^ 0x20 : (c))
#define TOUPPER(c)	((c) >= 'a' && (c) <= 'z' ? (c) ^ 0x20 : (c))

void bzero(void *s, size_t n);
void *memset(void *b, int c, size_t len);
void *memcpy(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strchr(const char *s, int c);
size_t strnlen(const char *s, size_t maxlen);
int atoi(const char *s);
uintptr_t strtop(const char *s);

#endif
