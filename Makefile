NAME = kfs.bin
CC = i686-elf-gcc
RM = /bin/rm -f

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): boot.o kernel.o
	$(CC) -T linker.ld -o $(NAME) -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

boot.o: boot.s
	i686-elf-as boot.s -o boot.o

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

clean:
	$(RM) boot.o kernel.o

fclean: clean
	$(RM) $(NAME)

re: fclean all
