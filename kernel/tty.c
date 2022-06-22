#include "tty.h"
#include "vga.h"
#include "io.h"
#include "string.h"

static const uint8_t VGA_WIDTH = 80;
static const uint8_t VGA_HEIGHT = 25;

uint8_t cursor_y;
uint8_t cursor_x;
uint8_t kcolor;
uint16_t *kbuffer;

void kinit(void)
{
	cursor_y = 0;
	cursor_x = 0;
	kcolor = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
	kbuffer = (uint16_t*)0xB8000;
	for (uint16_t y = 0; y < VGA_HEIGHT; y++) {
		for (uint16_t x = 0; x < VGA_WIDTH; x++) {
			const uint16_t index = y * VGA_WIDTH + x;
			kbuffer[index] = vga_entry(' ', kcolor);
		}
	}
}

void set_cursor(void)
{
	uint16_t pos = cursor_y * 80 + cursor_x;
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8));
    outb(0x3D4, 15);
    outb(0x3D5, pos);
}

void clear_screen(void)
{
	uint8_t clear_color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);

	for (uint16_t i = 0; i < VGA_WIDTH*VGA_HEIGHT; ++i)
		kbuffer[i] = vga_entry(' ', clear_color);
	cursor_x = 0;
	cursor_y = 0;
	set_cursor();
}

void scroll(void)
{
	if (cursor_y >= 25) {
		for (uint16_t i = 0; i < VGA_WIDTH * 24; ++i)
			kbuffer[i] = kbuffer[i + VGA_WIDTH];
		for (uint16_t i = 24*VGA_WIDTH; i < VGA_WIDTH * 25; ++i)
			kbuffer[i] = vga_entry(' ', kcolor);
		cursor_y = 24;
	}
}

void kputchar(char c)
{
	uint16_t index;

	if (c == '\n') {
		cursor_x = 0;
		++cursor_y;
	} else if (ISPRINT(c)) {
		index = cursor_y * VGA_WIDTH + cursor_x;
		kbuffer[index] = vga_entry(c, kcolor);
		++cursor_x;
	}
	if (cursor_x == VGA_WIDTH) {
		cursor_x = 0;
		++cursor_y;
	}

	scroll();
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

void banner(void)
{
	kcolor = vga_entry_color(VGA_LIGHT_GREEN, VGA_BLACK);
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
	kcolor = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
}
