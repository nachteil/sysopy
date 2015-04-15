CC=gcc
CC_FLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
RUN_DIR=run
BIN_DIR=bin

main: dirs main.o
	$(CC) -Wall -o $(BIN_DIR)/main $(OBJ_DIR)/main.o

main.o: $(SRC_DIR)/main.c $(SRC_DIR)/main.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/main.o $(SRC_DIR)/main.c

dirs:
	if [ ! -d $(SRC_DIR) ]; then mkdir $(SRC_DIR); fi
	if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi