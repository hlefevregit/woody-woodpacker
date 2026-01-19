NAME= woody-woodpacker

SRC=	src/main.c \
		src/elf_process.c \
# 		src/encryptor.c \
		src/injector.c \
		src/utils.c

OBJ= $(SRC:.c=.o)

ASM= nasm
ASMFLAGS= -f elf64

CC= gcc

CFLAGS= -Wall -Wextra -Werror -fno-pie -no-pie

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re