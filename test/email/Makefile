NAME=test
CFLAG=#-Werror -Wall -O3 -std=c99 -std=gnu99
SRC=
MAIN=main.c
INCLUDE=
LIB=-lcurl

$(NAME):
	$(CC) $(CFLAG) $(SRC) $(MAIN) -o $(NAME) $(INCLUDE) $(LIB)

clean:
	rm -rf $(NAME)

add:
	git add $(SRC) $(MAIN) Makefile
