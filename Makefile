NAME := kfs.bin
CC := i686-elf-gcc
CFLAGS := -Wall -Wextra -ffreestanding -O2 -I./kernel -I./libk
LFLAGS := -nodefaultlibs -nostdlib
SRCS := kernel/kernel.c \
		kernel/printk.c \
		kernel/ultoa.c \
		kernel/ujtoa.c \
		libk/memset.c \
		libk/memcpy.c \
		libk/memcmp.c \
		libk/strlen.c \
		libk/strchr.c \
		libk/strnlen.c
OBJS := $(SRCS:.c=.o)
ASM_SRCS := kernel/boot.s
ASM_OBJS := $(ASM_SRCS:.s=.o)
RM := /bin/rm -f

.PHONY: all clean fclean re iso

all: $(NAME)

iso: $(NAME)
	cp $(NAME) iso/boot/$(NAME)
	grub-mkrescue -o kfs.iso iso

$(NAME): $(ASM_OBJS) $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ $(CFLAGS) $(LFLAGS) $(ASM_OBJS) $(OBJS) -lgcc

$(ASM_OBJS): $(ASM_SRCS)
	i686-elf-as $< -o $@

clean:
	$(RM) $(ASM_OBJS) $(OBJS)

fclean: clean
	$(RM) $(NAME)
	$(RM) iso/boot/$(NAME)
	$(RM) kfs.iso

re: fclean all
