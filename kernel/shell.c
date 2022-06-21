#include "io.h"
#include "tty.h"
#include "string.h"
#include "shell.h"

static const char qwerty_kb_table[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a',
	's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x',
	'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',
	'0', '.', '6', 0, 0, 0, 0, 0
};

static const char qwerty_shift_kb_table[] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A',
	'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X',
	'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',
	'0', '.', '6', 0, 0, 0, 0, 0
};

static void getline(char *buf)
{
	uint16_t keycode = 0;
	int i = 0;
	int shift = 0;
	char c;

	while (1) {
		keycode = inb(KEYBOARD_PORT);

		if (keycode == 0x36 || keycode == 0x2A) {
			shift = 1;
			continue;
		} else if (keycode == 0xB6 || keycode == 0xAA) {
			shift = 0;
			continue;
		}
		if (keycode > 0 && keycode <= 128) {
			if (keycode == 0x1C) {
				kputchar('\n');
				outb(KEYBOARD_PORT, 0);
				return;
			}
			if (qwerty_kb_table[keycode] != 0) {
				if (shift) {
					c = qwerty_shift_kb_table[keycode];
					kputchar(c);
					buf[i] = c;
				} else {
					c = qwerty_kb_table[keycode];
					kputchar(c);
					buf[i] = c;
				}
				if (++i >= BUF_SIZE) {
					buf[BUF_SIZE-1] = '\0';
					outb(KEYBOARD_PORT, 0);
					return;
				}
			}
			outb(KEYBOARD_PORT, 0);
		}
	}
}

void shell(void)
{
	char buf[BUF_SIZE] = {0};

	while (1) {
		kputchar('$');
		kputchar(' ');
		getline(buf);
		if (strlen(buf) > 0) {
			memset(buf, 0, BUF_SIZE);
		}
	}
}
