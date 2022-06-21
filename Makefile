NAME := kfs.bin
CC := i686-elf-gcc
CFLAGS := -Wall -Wextra -ffreestanding -O2 -I./kernel -I./libc
LFLAGS := -nodefaultlibs -nostdlib
HDR := $(shell find kernel -type f -name *.h)
SRCS := kernel/kernel.c \
		kernel/printk.c \
		kernel/ultoa.c \
		kernel/ujtoa.c \
		kernel/gdt.c \
		kernel/shell.c \
		libc/memset.c \
		libc/memcpy.c \
		libc/memcmp.c \
		libc/strlen.c \
		libc/strchr.c \
		libc/strnlen.c
OBJS := $(SRCS:.c=.o)
ASM_SRCS := $(shell find kernel -type f -name *.s)
ASM_OBJS := $(ASM_SRCS:.s=.o)
RM := /bin/rm -f

.PHONY: all clean fclean re iso

all: $(NAME)

iso: $(NAME)
	cp $(NAME) iso/boot/$(NAME)
	grub-mkrescue -o kfs.iso iso

$(NAME): boot.o $(ASM_OBJS) $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ $(CFLAGS) $(LFLAGS) boot.o $(ASM_OBJS) $(OBJS) -lgcc

boot.o: boot.s
	i686-elf-as boot.s -o $@

%.o: %.s
	nasm -f elf32 $< -o $@

%.o: %.c $(HDR)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) boot.o $(ASM_OBJS) $(OBJS)

fclean: clean
	$(RM) $(NAME)
	$(RM) iso/boot/$(NAME)
	$(RM) kfs.iso

re: fclean all
