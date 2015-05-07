CC=gcc
CC_FLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
RUN_DIR=run
BIN_DIR=bin

all: rdfifo wrfifo

wrfifo: dirs wrfifo.o common.o
	$(CC) -Wall -o $(BIN_DIR)/wrfifo $(OBJ_DIR)/wrfifo.o $(OBJ_DIR)/common.o

wrfifo.o: $(SRC_DIR)/wrfifo.c $(SRC_DIR)/wrfifo.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/wrfifo.o $(SRC_DIR)/wrfifo.c

rdfifo: dirs rdfifo.o common.o
	$(CC) -Wall -o $(BIN_DIR)/rdfifo $(OBJ_DIR)/rdfifo.o $(OBJ_DIR)/common.o

rdfifo.o: $(SRC_DIR)/rdfifo.c $(SRC_DIR)/rdfifo.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/rdfifo.o $(SRC_DIR)/rdfifo.c

common.o: $(SRC_DIR)/common.c $(SRC_DIR)/common.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/common.o $(SRC_DIR)/common.c

dirs:
	if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi


clean:
	if [ -d $(OBJ_DIR) ]; then rm -rf $(OBJ_DIR); fi
	if [ -d $(BIN_DIR) ]; then rm -rf $(BIN_DIR); fi