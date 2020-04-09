LIB=-lwebsockets -lcrypto -lsqlite3 -lpython3.8 -lunqlite \
-lczmq -lzmq
CFLAG=-O6 -std=c99 -Wshadow -Wvla -pedantic -Wall
INCLUDE=-I header\
	-I /usr/include/python3.8

GOSRC=main.go

run:
	sudo go run $(GOSRC)

build:
	go build $(GOSRC)

install:
	mkdir -p object
	$(CC) -fPIC -c csrc/json.c -I cheader -o object/json.o
	$(CC) -fPIC -c csrc/msqlite.c -I cheader -o object/msqlite.o 
	$(CC) -fPIC -c csrc/webdatabase.c -I cheader -o object/webdatabase.o 
	$(CC) -fPIC -c csrc/mpython.c -I cheader -I /usr/local/include/python3.7m -o object/mpython.o 
	$(CC) -shared -o object/libclubhouse.so  object/json.o object/msqlite.o \
		object/webdatabase.o object/mpython.o -lsqlite3 -lpython3.7m
	mv object/libclubhouse.so /usr/local/lib/
	ldconfig

uninstall:
	rm /usr/local/lib/libclubhouse.so -rf
	ldconfig

clean:
	rm -rf main $(SQLITENAME) test object

add:
	git add header/* src/* Makefile \
	webdata.db www/css/* www/favicon/* www/fonts/* www/html/* www/img/* \
	www/index.html www/js/* www/scss/Makefile www/pdf/* www/scss/* py/*.py  \
	$(GOSRC) gosrc/* cheader/* csrc/* txt/*
