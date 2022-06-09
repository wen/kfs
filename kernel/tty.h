#ifndef TTY_H
#define TTY_H
#include <stddef.h>

void kputchar(char c);
void kwrite(const char *str, size_t size);
void kputstr(const char *str);

#endif
