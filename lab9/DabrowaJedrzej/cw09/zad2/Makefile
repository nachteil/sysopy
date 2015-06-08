CC=gcc
CC_FLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

all: rw

rw: dirs rw.o common.o
	$(CC) -Wall -o $(BIN_DIR)/rw $(OBJ_DIR)/rw.o  $(OBJ_DIR)/common.o -pthread

rw.o: $(SRC_DIR)/rw.c $(SRC_DIR)/rw.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/rw.o $(SRC_DIR)/rw.c

common.o: $(SRC_DIR)/common.c $(SRC_DIR)/common.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/common.o $(SRC_DIR)/common.c

dirs:
	if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi

clean:
	if [ -d $(OBJ_DIR) ]; then rm -rf $(OBJ_DIR); fi
	if [ -d $(BIN_DIR) ]; then rm -rf $(BIN_DIR); fi