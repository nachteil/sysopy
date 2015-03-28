
CC=gcc
CFLAGS=-c -Wall
OBJ_DIR=obj
SRC_DIR=src

all: dirs run_static

run_static: make_static
	./obj/main

run_dynamic_1: make_dynamic
	./obj/main

dirs:
	if [ ! -d obj ]; then mkdir obj; fi
	if [ ! -d lib ]; then mkdir lib; fi

main: main.o list_funs.o list_structs.o utils.o
	$(CC) $(OBJ_DIR)/main.o $(OBJ_DIR)/list_funs.o $(OBJ_DIR)/list_structs.o $(OBJ_DIR)/utils.o -o $(OBJ_DIR)/main

lib_static: list_funs.o list_structs.o utils.o
	ar crs static_main_lib.a $(OBJ_DIR)/list_funs.o $(OBJ_DIR)/list_structs.o $(OBJ_DIR)/utils.o
	mv static_main_lib.a ./lib

lib_dynamic:
	$(CC) -fPIC $(CFLAGS) ./$(SRC_DIR)/list_funs.c -o $(OBJ_DIR)/list_funs.o
	$(CC) -fPIC $(CFLAGS) ./$(SRC_DIR)/list_structs.c -o $(OBJ_DIR)/list_structs.o
	$(CC) -fPIC $(CFLAGS) ./$(SRC_DIR)/utils.c -o $(OBJ_DIR)/utils.o
	$(CC) –shared –fPIC –o ./lib/libdynamic_main_lib.so $(OBJ_DIR)/utils.o $(OBJ_DIR)/list_structs.o $(OBJ_DIR)/list_funs.o

make_dynamic: lib_dynamic
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(OBJ_DIR)/main -L./lib -ldynamic_main_lib
	chmod +x $(OBJ_DIR)/main

make_static: lib_static main.o
	$(CC) $(OBJ_DIR)/main.o ./lib/static_main_lib.a -o $(OBJ_DIR)/main

main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

list_funs.o: $(SRC_DIR)/list_funs.c
	$(CC) $(CFLAGS) $(SRC_DIR)/list_funs.c -o $(OBJ_DIR)/list_funs.o

list_structs.o: $(SRC_DIR)/list_structs.c
	$(CC) $(CFLAGS) $(SRC_DIR)/list_structs.c -o $(OBJ_DIR)/list_structs.o

utils.o: $(SRC_DIR)/utils.c
	$(CC) $(CFLAGS) $(SRC_DIR)/utils.c -o $(OBJ_DIR)/utils.o

clean:
	rm ./$(OBJ_DIR)/*.o ./$(OBJ_DIR)/main ./lib/*