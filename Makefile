NAME=main
SRC=main.c\
	src/apihandler.c\
	src/gethandler.c\
	src/webhandler.c\
	src/json.c
LIB=-lwebsockets
CFLAG=-O3 -std=c99 -Wall -Wshadow -Wvla -pedantic
INCLUDE=header

$(NAME):
	$(CC) $(SRC) $(CFLAG) -I $(INCLUDE) -o $(NAME) $(LIB)

clean:
	rm -rf $(NAME)

add:
	git add header/* $(SRC) Makefile
