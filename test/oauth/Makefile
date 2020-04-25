NAME=test
CFLAG=-Wall -std=c99 -std=gnu99
# -Werror -O3
SRC=oauth.c
MAIN=main.c
INCLUDE=
LIB=-ljwt -lcrypto -ljansson -lcheck -lpthread -lm -lrt -lsqlite3 -lsubunit

$(NAME):
	$(CC) $(CFLAG) $(SRC) $(MAIN) -o $(NAME) $(INCLUDE) $(LIB)

clean:
	rm -rf $(NAME)

add:
	git add $(SRC) $(MAIN) Makefile oauth.h
