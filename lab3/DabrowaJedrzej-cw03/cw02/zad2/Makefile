CC=gcc
CC_FLAGS=-Wall -c
SRC_DIR=./src
OBJ_DIR=./obj
BIN_DIR=./bin

all: clear dirs file_counter

file_counter: file_counter.o
	@$(CC) -Wall -o $(BIN_DIR)/file_counter $(OBJ_DIR)/file_counter.o

file_counter.o: $(SRC_DIR)/file_counter.c $(SRC_DIR)/file_counter.h
	@$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/file_counter.o $(SRC_DIR)/file_counter.c

dirs:
	@if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi;
	@if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi;

clear:
	@rm -rf $(BIN_DIR)
	@rm -rf $(OBJ_DIR)