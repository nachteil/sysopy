CC=gcc
CCFLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
RUN_DIR=run
OUT_DIR=output

all: clear dirs main

run: all
	@./run.sh $(OBJ_DIR) 1 > $(OUT_DIR)/fork.csv &
	@./run.sh $(OBJ_DIR) 2 > $(OUT_DIR)/vfork.csv &
	@./run.sh $(OBJ_DIR) 3 > $(OUT_DIR)/fork_clone.csv &
	@./run.sh $(OBJ_DIR) 4 > $(OUT_DIR)/vfork_vlone.csv &

main: main.o
	@$(CC) -Wall -o ./$(OBJ_DIR)/main ./$(OBJ_DIR)/main.o

main.o: $(SRC_DIR)/main.c $(SRC_DIR)/main.h
	@$(CC) $(CCFLAGS) -o ./$(OBJ_DIR)/main.o $(SRC_DIR)/main.c

clear:
	@rm -rf $(OBJ_DIR)
	@rm -rf $(RUN_DIR)
	@rm -rf $(OUT_DIR)

dirs:
	@@if [ ! -d ./$(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi;
	@@if [ ! -d ./$(OUT_DIR) ]; then mkdir $(OUT_DIR); fi;
	@@if [ ! -d ./$(RUN_DIR) ]; then mkdir $(RUN_DIR); fi;
	chmod +x run.sh