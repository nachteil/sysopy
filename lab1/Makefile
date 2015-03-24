
CC=gcc
CFLAGS=-c -Wall
OBJ_DIR=obj
SRC_DIR=src

all: main

main: main.o list_funs.o list_structs.o utils.o
	$(CC) $(OBJ_DIR)/main.o $(OBJ_DIR)/list_funs.o $(OBJ_DIR)/list_structs.o $(OBJ_DIR)/utils.o -o $(OBJ_DIR)/main

main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

list_funs.o: $(SRC_DIR)/list_funs.c
	$(CC) $(CFLAGS) $(SRC_DIR)/list_funs.c -o $(OBJ_DIR)/list_funs.o

list_structs.o: $(SRC_DIR)/list_structs.c
	$(CC) $(CFLAGS) $(SRC_DIR)/list_structs.c -o $(OBJ_DIR)/list_structs.o

utils.o: $(SRC_DIR)/utils.c
	$(CC) $(CFLAGS) $(SRC_DIR)/utils.c -o $(OBJ_DIR)/utils.o

clean:
	rm ./$(OBJ_DIR)/*.o ./$(OBJ_DIR)/main