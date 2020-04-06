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
CFLAG=-O6 -std=c99 -Wshadow -Wvla -pedantic -Wall
INCLUDE=-I header\
	-I /usr/include/python3.8

GOSRC=main.go

build:
	go build $(GOSRC)

install:
	mkdir -p object
	$(CC) -fPIC -c csrc/json.c -I cheader -o object/json.o
	$(CC) -fPIC -c csrc/msqlite.c -I cheader -o object/msqlite.o 
	$(CC) -fPIC -c csrc/webdatabase.c -I cheader -o object/webdatabase.o 
	$(CC) -shared -o object/libclubhouse.so  object/json.o object/msqlite.o object/webdatabase.o -lsqlite3
	cp object/libclubhouse.so /usr/local/lib/
	ldconfig

uninstall:
	rm /usr/local/lib/libclubhouse.so -rf

clean:
	rm -rf $(NAME) $(SQLITENAME) test object

add:
	git add header/* $(SRC) Makefile $(NAME).c $(SQLITENAME).c \
	webdata.db www/css/* www/favicon/* www/fonts/* www/html/* www/img/* \
	www/index.html www/js/* www/scss/Makefile www/pdf/* www/scss/* py/*  \
	$(GOSRC) gosrc/* cheader/* csrc/*
