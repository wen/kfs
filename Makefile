NAME := kfs.bin
CC := i686-elf-gcc
CFLAGS := -Wall -Wextra -ffreestanding -O2 -I. -I./libk
LFLAGS := -nodefaultlibs -nostdlib
SRCS := kernel.c \
		libk/memset.c \
		libk/memcpy.c \
		libk/memcmp.c \
		libk/strlen.c \
		libk/putchar.c \
		libk/printf.c
OBJS := $(SRCS:.c=.o)
RM := /bin/rm -f

.PHONY: all clean fclean re iso

all: $(NAME)

iso: $(NAME)
	cp $(NAME) iso/boot/$(NAME)
	grub-mkrescue -o kfs.iso iso

$(NAME): $(OBJS) linker.ld boot.o
	$(CC) -T linker.ld -o $@ $(CFLAGS) $(LFLAGS) boot.o $(OBJS) -lgcc

boot.o: boot.s
	i686-elf-as boot.s -o boot.o

clean:
	$(RM) boot.o $(OBJS)

fclean: clean
	$(RM) $(NAME)
	$(RM) iso/boot/$(NAME)
	$(RM) kfs.iso

re: fclean all
