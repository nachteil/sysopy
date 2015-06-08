CC=gcc
CC_FLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

main: dirs main.o common.o
	$(CC) -Wall -o $(BIN_DIR)/main $(OBJ_DIR)/main.o -pthread

main.o: $(SRC_DIR)/main.c $(SRC_DIR)/main.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/main.o $(SRC_DIR)/main.c

common.o: $(SRC_DIR)/common.c $(SRC_DIR)/common.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/common.o $(SRC_DIR)/common.c

dirs:
	if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi

clean:
	if [ -d $(OBJ_DIR) ]; then rm -rf $(OBJ_DIR); fi
	if [ -d $(BIN_DIR) ]; then rm -rf $(BIN_DIR); fi