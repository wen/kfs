#ifndef TTY_H
#define TTY_H

#include <stddef.h>

#define EOF		(-1)

void kputchar(char c);
void kwrite(const char *str, size_t size);
void kputstr(const char *str);
int printk(const char *fmt, ...);

#endif
