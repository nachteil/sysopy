CC=gcc
CC_FLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
MSG="Executables for client and server can be found in bin/ directory"

all: clean server client readme
	@echo
	@echo $(MSG)
	@echo

server: dirs server.o common.o communication.o
	$(CC) -Wall -o $(BIN_DIR)/server $(OBJ_DIR)/server.o $(OBJ_DIR)/common.o $(OBJ_DIR)/communication.o -lrt

server.o: $(SRC_DIR)/server.c $(SRC_DIR)/server.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/server.o $(SRC_DIR)/server.c

client: dirs client.o common.o communication.o
	$(CC) -Wall -o $(BIN_DIR)/client $(OBJ_DIR)/client.o $(OBJ_DIR)/common.o $(OBJ_DIR)/communication.o -lrt

client.o: $(SRC_DIR)/client.c $(SRC_DIR)/client.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/client.o $(SRC_DIR)/client.c

common.o: $(SRC_DIR)/common.c $(SRC_DIR)/common.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/common.o $(SRC_DIR)/common.c

communication.o: $(SRC_DIR)/communication.c $(SRC_DIR)/communication.h
	$(CC) $(CC_FLAGS) -o $(OBJ_DIR)/communication.o $(SRC_DIR)/communication.c

dirs:
	@if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	@if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi

readme:
	@touch readme
	@echo $(MSG) > readme

clean:
	@if [ -d $(OBJ_DIR) ]; then rm -rf $(OBJ_DIR); fi
	@if [ -d $(BIN_DIR) ]; then rm -rf $(BIN_DIR); fi
	@rm -f /tmp/jdabrowa*
	@rm -f readme
	@clear
