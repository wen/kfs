#include <stddef.h>
#include "vga.h"
#include "tty.h"
#include "string.h"
#include "io.h"
#include "stack.h"
#include "gdt.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t krow;
size_t kcolumn;
uint8_t kcolor;
uint16_t *kbuffer;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

void kinit(void)
{
	krow = 0;
	kcolumn = 0;
	kcolor = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
	kbuffer = (uint16_t*)0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			kbuffer[index] = vga_entry(' ', kcolor);
		}
	}
}

void set_cursor(void)
{
	uint16_t pos = krow * 80 + kcolumn;
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8));
    outb(0x3D4, 15);
    outb(0x3D5, pos);
}

void kputchar(char c)
{
	size_t index;

	if (c == '\n') {
		kcolumn = 0;
		++krow;
	} else {
		index = krow * VGA_WIDTH + kcolumn;
		kbuffer[index] = vga_entry(c, kcolor);
		++kcolumn;
	}
	if (kcolumn == VGA_WIDTH) {
		kcolumn = 0;
	}
	set_cursor();
}

void kwrite(const char *str, size_t size)
{
	for (size_t i = 0; i < size; ++i)
		kputchar(str[i]);
}

void kputstr(const char *str)
{
	char *ptr = (char*)str;

	while (*ptr)
		kputchar(*ptr++);
}

static void banner(void)
{
	kcolor = vga_entry_color(VGA_LIGHT_CYAN, VGA_BLACK);
	kputstr("@@@  @@@  @@@@@@@@   @@@@@@           @@@    @@@@@@\n");
	kputstr("@@@  @@@  @@@@@@@@  @@@@@@@          @@@@   @@@@@@@@\n");
	kputstr("@@!  !@@  @@!       !@@             @@!@!        @@@\n");
	kputstr("!@!  @!!  !@!       !@!            !@!!@!       @!@\n");
	kputstr("@!@@!@!   @!!!:!    !!@@!!        @!! @!!      !!@\n");
	kputstr("!!@!!!    !!!!!:     !!@!!!      !!!  !@!     !!:\n");
	kputstr("!!: :!!   !!:            !:!     :!!:!:!!:   !:!\n");
	kputstr(":!:  !:!  :!:           !:!      !:::!!:::  :!:\n");
	kputstr(" ::  :::   ::       :::: ::           :::   :: :::::\n");
	kputstr(" :   :::   :        :: : :            :::   :: : :::\n\n");
}

void kernel_main(void)
{
	kinit();
	banner();

	kcolor = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
	printk("%08x\n", get_esp());
	printk("%08x\n", get_ebp());
	//printk("%c%c", '$', ' ');
}
