#include "shell.h"
#include "io.h"
#include "tty.h"
#include "string.h"
#include "stack.h"
#include "gdt.h"

static const char qwerty_kb_table[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a',
	's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x',
	'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',
	'0', '.', 0, 0, 0, 0, 0, 0
};

static const char qwerty_shift_kb_table[] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A',
	'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X',
	'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',
	'0', '.', 0, 0, 0, 0, 0, 0
};

void reboot()
{
	uint8_t good = 0x02;

	while (good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	shutdown();
}

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
		if (keycode > 0 && keycode <= 0x58) {
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
					kputchar('\n');
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
	uintptr_t ebp;
	uintptr_t esp;

	while (1) {
		kputstr("kfs $ ");
		getline(buf);
		if (strlen(buf) > 0) {
			if (!strcmp(buf, "halt") || !strcmp(buf, "shutdown")) {
				shutdown();
			} else if (!strcmp(buf, "reboot")) {
				reboot();
			} else if (!strcmp(buf, "clear")) {
				clear_screen();
			} else if (!strcmp(buf, "stack")) {
				GET_EBP(ebp);
				GET_ESP(esp);
				printk("EBP: 0x%08X  ESP: 0x%08X\n", ebp, esp);
				print_memory((void*)esp, ebp - esp);
			} else if (!strcmp(buf, "gdt")) {
				print_memory((void*)GDT_PTR, 32);
			} else if (!memcmp(buf, "dump", 4) && strlen(buf+5) == 8) {
				print_memory((void*)strtop(buf+5), 128);
			}
			bzero(buf, BUF_SIZE);
		}
	}
}
