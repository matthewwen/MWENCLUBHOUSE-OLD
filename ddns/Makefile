NAME=ddns
SRC=main.c
LIB=-lgetdns

$(NAME):
	$(CC) $(SRC) -o $(NAME) $(LIB)

clean:
	rm -rf $(NAME)

add:
	git add $(SRC) Makefile
