NAME=main
SQLITENAME=mainsqlite
SRC=src/apihandler.c\
    src/gethandler.c\
    src/posthandler.c\
    src/puthandler.c\
    src/webhandler.c\
    src/json.c\
    src/security.c\
    src/common.c \
    src/msqlite.c\
    src/webdatabase.c \
    src/moreyIOT.c
LIB=-lwebsockets -lcrypto -lsqlite3
#-lguile -lltdl -lssl -pthread -lgmp -lcrypt -lm
CFLAG=-O6 -std=c99 -Wall -Wshadow -Wvla -pedantic
INCLUDE=header

$(NAME):
	$(CC) $(NAME).c $(SRC) $(CFLAG) -I $(INCLUDE) -o $(NAME) $(LIB)

$(SQLITENAME): 
	$(CC) $(SQLITENAME).c $(SRC) -I $(INCLUDE) -o $(SQLITENAME) $(LIB) -g

clean:
	rm -rf $(NAME) $(SQLITENAME)

add:
	git add header/* $(SRC) Makefile $(NAME).c $(SQLITENAME).c webdata.db www/css/* www/favicon/* www/fonts/* www/html/* www/img/* www/index.html www/js/* www/scss/Makefile www/pdf/* www/scss/* py/*
