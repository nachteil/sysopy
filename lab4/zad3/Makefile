CC=gcc
CC_FLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
RUN_DIR=run
BIN_DIR=bin

all: dirs child parent

child: child.o common.o
	$(CC) -Wall -o $(BIN_DIR)/child $(OBJ_DIR)/child.o $(OBJ_DIR)/common.o

parent: parent.o common.o
	$(CC) -Wall -o $(BIN_DIR)/parent $(OBJ_DIR)/parent.o $(OBJ_DIR)/common.o


child.o: $(SRC_DIR)/child.c $(SRC_DIR)/child.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/child.o $(SRC_DIR)/child.c

parent.o: $(SRC_DIR)/parent.c $(SRC_DIR)/parent.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/parent.o $(SRC_DIR)/parent.c


common.o: $(SRC_DIR)/common.c $(SRC_DIR)/common.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/common.o $(SRC_DIR)/common.c

dirs:
	if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi


clean:
	if [ -d $(OBJ_DIR) ]; then rm -rf $(OBJ_DIR); fi
	if [ -d $(BIN_DIR) ]; then rm -rf $(BIN_DIR); fi