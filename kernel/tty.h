#ifndef TTY_H
#define TTY_H

#include <stddef.h>

#define EOF (-1)

void kinit(void);
void clear_screen(void);
void kputchar(char c);
void kwrite(const char *str, size_t size);
void kputstr(const char *str);
int printk(const char *fmt, ...);
void banner(void);

#endif
