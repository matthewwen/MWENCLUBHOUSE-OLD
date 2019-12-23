SERVER_NAME=server
CLIENT_NAME=client

SRC_S=$(SERVER_NAME).c \
      src/http.c
SRC_C=$(CLIENT_NAME).c

MAIN_NAME=main
MAIN_C=$(MAIN_NAME).c
LIB=-lmicrohttpd

SERVER_INCLUDE=include

all: $(MAIN_NAME)

$(MAIN_NAME):
	$(CC) $(MAIN_C) -o $(MAIN_NAME) $(LIB)

$(SERVER_NAME):
	$(CC) $(SRC_S) -o $(SERVER_NAME) -I $(SERVER_INCLUDE) -g

$(CLIENT_NAME):
	$(CC) $(SRC_C) -o $(CLIENT_NAME) -lcurl

clean:
	rm -rf $(SERVER_NAME) $(CLIENT_NAME) $(MAIN_NAME)

add:
	git add $(SRC_S) $(SRC_C) Makefile $(MAIN_C)
