
CC=gcc
CFLAGS=-c -Wall
OBJ_DIR=obj
SRC_DIR=src
LIB_DIR=lib

all: dirs lib_static lib_shared

dirs:
	@if [ ! -d obj ]; then mkdir obj; fi
	@if [ ! -d lib ]; then mkdir lib; fi

lib_static: list_funs.o list_structs.o utils.o
	@ar crs static_main_lib.a $(OBJ_DIR)/list_funs.o $(OBJ_DIR)/list_structs.o $(OBJ_DIR)/utils.o
	@mv static_main_lib.a ./lib

lib_shared:
	@$(CC) -Werror -fPIC $(CFLAGS) ./$(SRC_DIR)/list_funs.c -o $(OBJ_DIR)/list_funs.o
	@$(CC) -Werror -fPIC $(CFLAGS) ./$(SRC_DIR)/list_structs.c -o $(OBJ_DIR)/list_structs.o
	@$(CC) -Werror -fPIC $(CFLAGS) ./$(SRC_DIR)/utils.c -o $(OBJ_DIR)/utils.o
	@$(CC) -Werror -fPIC -shared -o ./$(LIB_DIR)/libshared_main_lib.so $(OBJ_DIR)/utils.o $(OBJ_DIR)/list_structs.o $(OBJ_DIR)/list_funs.o

make_shared: lib_shared
	@$(CC) -Wall $(SRC_DIR)/main.c -o $(OBJ_DIR)/main -L./lib  -Wl,-rpath=lib -lshared_main_lib
	@chmod +x $(OBJ_DIR)/main

make_static: lib_static main.o
	@$(CC) $(OBJ_DIR)/main.o ./$(LIB_DIR)/static_main_lib.a -o $(OBJ_DIR)/main

make_dynamic: lib_shared
	@$(CC) -Wall -L./lib ./$(SRC_DIR)/main_dynamic.c -lshared_main_lib -o ./$(OBJ_DIR)/main_dynamic -ldl

list_funs.o: $(SRC_DIR)/list_funs.c
	@$(CC) $(CFLAGS) $(SRC_DIR)/list_funs.c -o $(OBJ_DIR)/list_funs.o

list_structs.o: $(SRC_DIR)/list_structs.c
	@$(CC) $(CFLAGS) $(SRC_DIR)/list_structs.c -o $(OBJ_DIR)/list_structs.o

utils.o: $(SRC_DIR)/utils.c
	@$(CC) $(CFLAGS) $(SRC_DIR)/utils.c -o $(OBJ_DIR)/utils.o

clean:
	@rm -rf $(OBJ_DIR)
	@rm -rf $(LIB_DIR)
