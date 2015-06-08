CC=gcc
CC_FLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

all: client server

client: dirs client.o common.o
	$(CC) -Wall -o $(BIN_DIR)/client $(OBJ_DIR)/client.o $(OBJ_DIR)/common.o -pthread

client.o: $(SRC_DIR)/client.c $(SRC_DIR)/client.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/client.o $(SRC_DIR)/client.c
	
server: dirs server.o common.o
	$(CC) -Wall -o $(BIN_DIR)/server $(OBJ_DIR)/server.o $(OBJ_DIR)/common.o

server.o: $(SRC_DIR)/server.c $(SRC_DIR)/server.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/server.o $(SRC_DIR)/server.c

common.o: $(SRC_DIR)/common.c $(SRC_DIR)/common.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/common.o $(SRC_DIR)/common.c

dirs:
	if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi

clean:
	if [ -d $(OBJ_DIR) ]; then rm -rf $(OBJ_DIR); fi
	if [ -d $(BIN_DIR) ]; then rm -rf $(BIN_DIR); fi