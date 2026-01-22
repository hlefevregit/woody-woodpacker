NAME= woody-woodpacker

SRC=	src/main.c \
		src/elf_process.c \

ASM_SRC=	src/encrypt.asm

OBJ= $(SRC:.c=.o)
ASM_OBJ = $(ASM_SRC:.asm=.o)

ASM= nasm
ASMFLAGS= -f elf64

CC= gcc

CFLAGS= -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJ) $(ASM_OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(ASM_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(ASM_OBJ)

fclean: clean
	rm -f $(NAME) woody

re: fclean all

.PHONY: all clean fclean re