CC=gcc
CCFLAGS=-Wall -c -std=c99
SRC_DIR=src
OBJ_DIR=obj
RUN_DIR=run

all: generate sort

sort: make_sorter copy_res
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_1_std.bin 4 LIB
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_1_sys.bin 4 SYS
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_2_std.bin 4 LIB
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_2_sys.bin 4 SYS
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_3_std.bin 512 LIB
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_3_sys.bin 512 SYS
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_4_std.bin 512 LIB
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_4_sys.bin 512 SYS
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_5_std.bin 4096 LIB
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_5_sys.bin 4096 SYS
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_6_std.bin 4096 LIB
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_6_sys.bin 4096 SYS
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_7_std.bin 8192 LIB
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_7_sys.bin 8192 SYS
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_8_std.bin 8192 LIB
	@./$(RUN_DIR)/bubble_sorter ./$(RUN_DIR)/set_8_sys.bin 8192 SYS

copy_res:
	@cp ./$(RUN_DIR)/set_1.bin ./$(RUN_DIR)/set_1_sys.bin
	@cp ./$(RUN_DIR)/set_1.bin ./$(RUN_DIR)/set_1_std.bin
	@cp ./$(RUN_DIR)/set_2.bin ./$(RUN_DIR)/set_2_sys.bin
	@cp ./$(RUN_DIR)/set_2.bin ./$(RUN_DIR)/set_2_std.bin
	@cp ./$(RUN_DIR)/set_3.bin ./$(RUN_DIR)/set_3_sys.bin
	@cp ./$(RUN_DIR)/set_3.bin ./$(RUN_DIR)/set_3_std.bin
	@cp ./$(RUN_DIR)/set_4.bin ./$(RUN_DIR)/set_4_sys.bin
	@cp ./$(RUN_DIR)/set_4.bin ./$(RUN_DIR)/set_4_std.bin
	@cp ./$(RUN_DIR)/set_5.bin ./$(RUN_DIR)/set_5_sys.bin
	@cp ./$(RUN_DIR)/set_5.bin ./$(RUN_DIR)/set_5_std.bin
	@cp ./$(RUN_DIR)/set_6.bin ./$(RUN_DIR)/set_6_sys.bin
	@cp ./$(RUN_DIR)/set_6.bin ./$(RUN_DIR)/set_6_std.bin
	@cp ./$(RUN_DIR)/set_7.bin ./$(RUN_DIR)/set_7_sys.bin
	@cp ./$(RUN_DIR)/set_7.bin ./$(RUN_DIR)/set_7_std.bin
	@cp ./$(RUN_DIR)/set_8.bin ./$(RUN_DIR)/set_8_sys.bin
	@cp ./$(RUN_DIR)/set_8.bin ./$(RUN_DIR)/set_8_std.bin

generate: dirs make_generator
	@./$(OBJ_DIR)/records_generator ./$(RUN_DIR)/set_1.bin 4 6000
	@./$(OBJ_DIR)/records_generator ./$(RUN_DIR)/set_2.bin 4 12000
	@./$(OBJ_DIR)/records_generator ./$(RUN_DIR)/set_3.bin 512 2000
	@./$(OBJ_DIR)/records_generator ./$(RUN_DIR)/set_4.bin 512 10000
	@./$(OBJ_DIR)/records_generator ./$(RUN_DIR)/set_5.bin 4096 2000
	@./$(OBJ_DIR)/records_generator ./$(RUN_DIR)/set_6.bin 4096 10000
	@./$(OBJ_DIR)/records_generator ./$(RUN_DIR)/set_7.bin 8192 2000
	@./$(OBJ_DIR)/records_generator ./$(RUN_DIR)/set_8.bin 8192 10000

dirs:
	@if [ ! -d ./$(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi;
	@if [ ! -d ./$(RUN_DIR) ]; then mkdir $(RUN_DIR); fi;

make_generator: dirs records_generator.o
	@$(CC) -Wall ./$(OBJ_DIR)/records_generator.o -o ./$(OBJ_DIR)/records_generator

make_sorter: dirs bubble_sorter.o bubble_sort.o
	@$(CC) -Wall ./$(OBJ_DIR)/bubble_sorter.o ./$(OBJ_DIR)/bubble_sort.o -o ./$(OBJ_DIR)/bubble_sorter
	@cp ./$(OBJ_DIR)/bubble_sorter ./$(RUN_DIR)/


bubble_sorter.o: src/bubble_sorter.c
	@$(CC) $(CCFLAGS) ./$(SRC_DIR)/bubble_sorter.c -o ./$(OBJ_DIR)/bubble_sorter.o

bubble_sort.o: src/bubble_sort.c
	@$(CC) $(CCFLAGS) ./$(SRC_DIR)/bubble_sort.c -o ./$(OBJ_DIR)/bubble_sort.o

records_generator.o: src/records_generator.c
	@$(CC) $(CCFLAGS) ./$(SRC_DIR)/records_generator.c -o ./$(OBJ_DIR)/records_generator.o

clear:
	@rm -rf obj
	@rm -rf run