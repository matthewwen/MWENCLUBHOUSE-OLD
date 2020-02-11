NAME=main
SRC=main.c\
    src/apihandler.c\
    src/gethandler.c\
    src/posthandler.c\
    src/puthandler.c\
    src/webhandler.c\
    src/json.c\
    src/security.c\
    src/common.c \
    src/msqlite.c
LIB=-lwebsockets -lcrypto -lsqlite3
#-lguile -lltdl -lssl -pthread -lgmp -lcrypt -lm
CFLAG=-O3 -std=c99 -Wall -Wshadow -Wvla -pedantic
INCLUDE=header

$(NAME):
	$(CC) $(SRC) $(CFLAG) -I $(INCLUDE) -o $(NAME) $(LIB)

clean:
	rm -rf $(NAME)

add:
	git add header/* $(SRC) Makefile www/js/* www/css/* www/* www/img/* www/favicon/* data.db
