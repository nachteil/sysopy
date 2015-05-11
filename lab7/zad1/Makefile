CC=gcc
CC_FLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

MESSAGE="Producer and consumer executables can be found in ./bin directory, run ./bin/cleanup to clean IPC objects"

all: consumer producer cleanup
	@echo
	@echo $(MESSAGE)
	@echo

consumer: dirs consumer.o common.o lab7ipc.o
	$(CC) -Wall -o $(BIN_DIR)/consumer $(OBJ_DIR)/consumer.o $(OBJ_DIR)/common.o $(OBJ_DIR)/lab7ipc.o -lrt

consumer.o: $(SRC_DIR)/consumer.c $(SRC_DIR)/consumer.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/consumer.o $(SRC_DIR)/consumer.c

producer: dirs producer.o common.o lab7ipc.o
	$(CC) -Wall -o $(BIN_DIR)/producer $(OBJ_DIR)/producer.o $(OBJ_DIR)/common.o $(OBJ_DIR)/lab7ipc.o -lrt

producer.o: $(SRC_DIR)/producer.c $(SRC_DIR)/producer.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/producer.o $(SRC_DIR)/producer.c

common.o: $(SRC_DIR)/common.c $(SRC_DIR)/common.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/common.o $(SRC_DIR)/common.c
	
lab7ipc.o: $(SRC_DIR)/lab7ipc.c $(SRC_DIR)/lab7ipc.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/lab7ipc.o $(SRC_DIR)/lab7ipc.c
	
cleanup.o: $(SRC_DIR)/cleanup.c
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/cleanup.o $(SRC_DIR)/cleanup.c

cleanup: dirs cleanup.o lab7ipc.o common.o
	$(CC) -Wall -o $(BIN_DIR)/cleanup $(OBJ_DIR)/lab7ipc.o $(OBJ_DIR)/cleanup.o $(OBJ_DIR)/common.o -lrt

dirs:
	if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi

clean:
	if [ -d $(OBJ_DIR) ]; then rm -rf $(OBJ_DIR); fi
	if [ -d $(BIN_DIR) ]; then rm -rf $(BIN_DIR); fi
	ipcs -s | tail -n+4 | awk -F' ' '{ print $1 }' | xargs -r -L 1 ipcrm -S
