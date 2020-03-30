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
    src/webdatabase.c\
    src/munqlite.c\
    src/moreyIOT.c\
    src/todo.c\
    src/mqtthandler.c

LIB=-lwebsockets -lcrypto -lsqlite3 -lpython3.8 -lunqlite \
-lczmq -lzmq
#-lguile -lltdl -lssl -pthread -lgmp -lcrypt -lm
CFLAG=-O6 -std=c99 -Wshadow -Wvla -pedantic -Wall
#CFLAG=-O6 -std=c99 
INCLUDE=-I header\
	-I /usr/include/python3.8

$(NAME):
	$(CC) $(NAME).c $(SRC) $(CFLAG) -DALLOWPYTHON $(INCLUDE) -o $(NAME) $(LIB)

test: 
	$(CC) $(NAME).c $(SRC) $(CFLAG) -DTESTDEPLOYMENT $(INCLUDE) -o $(NAME) $(LIB)

$(SQLITENAME): 
	$(CC) $(SQLITENAME).c $(SRC) -I $(INCLUDE) -o $(SQLITENAME) $(LIB) -g

clean:
	rm -rf $(NAME) $(SQLITENAME) test

add:
	git add header/* $(SRC) Makefile $(NAME).c $(SQLITENAME).c webdata.db www/css/* www/favicon/* www/fonts/* www/html/* www/img/* www/index.html www/js/* www/scss/Makefile www/pdf/* www/scss/* py/* 
