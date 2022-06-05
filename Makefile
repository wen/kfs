NAME = kfs.bin
CC = i686-elf-gcc
CFLAGS = -Wall -ffreestanding -O2
LFLAGS = -nodefaultlibs -nostdlib
RM = /bin/rm -f

.PHONY: all clean fclean re iso

all: $(NAME)

iso: $(NAME)
	cp $(NAME) iso/boot/$(NAME)
	grub-mkrescue -o kfs.iso iso

$(NAME): linker.ld boot.o kernel.o
	$(CC) $(CFLAGS) $(LFLAGS) -T linker.ld -o $@ boot.o kernel.o

boot.o: boot.s
	i686-elf-as boot.s -o boot.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) boot.o kernel.o

fclean: clean
	$(RM) $(NAME)
	$(RM) iso/boot/$(NAME)
	$(RM) kfs.iso

re: fclean all
