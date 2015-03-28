//
// Created by yorg on 17.03.15.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "bubble_sorter.h"

#define EXPECTED_ARGUMENTS_COUNT 3
#define ERROR_EXIT_CODE 1

#define SYS_IO 3534533
#define STD_IO 88345

int io_type;

FILE * stdio_file_handler = NULL;
int sys_file_descriptor = -1;

void usage() {
    printf("USAGE:\n\tbubble_sorter RECF_PATH RECLEN TYPE\n");
    printf("\tRECF_PATH - path to a file containing records to sort\n");
    printf("\tRECLEN - size (in bytes) of a single record\n");
    printf("\tTYPE - in/out functions to use in program:\n");
    printf("\t\tSYS - using system io\n");
    printf("\t\tLIB - using library io\n");
}


char **get_two_records(int first_rec_index, int rec_size_in_bytes) {

    if(io_type == SYS_IO) {
        return read_two_records_using_sys_io(first_rec_index, rec_size_in_bytes);
    } else {
        return read_two_records_using_std_io(first_rec_index, rec_size_in_bytes);
    }
}

char ** get_two_records_using_sys_io(int first_rec_index, int rec_size_in_bytes) {

    char ** buf = (char**) malloc(2);
    buf[0] = (char *) malloc((size_t) rec_size_in_bytes);
    buf[1] = (char *) malloc((size_t) rec_size_in_bytes);

    if( buf == NULL || buf[0] == NULL || buf[1] == NULL) {
        printf("Memory allocation error\n");
        exit(ERROR_EXIT_CODE);
    }


}

char ** get_two_records_using_std_io(int first_rec_index, int rec_size_in_bytes) {

}

int main(int argc, char * argv[])
{
    if(argc != EXPECTED_ARGUMENTS_COUNT + 1) {
        usage();
        exit(ERROR_EXIT_CODE);
    }

    if (strcmp(argv[3], "SYS") == 0) {

        io_type = SYS_IO;

        if((stdio_file_handler = fopen(argv[1], "rwb")) == NULL) {
            printf("The file %s could not have been opened\n", argv[1]);
            exit(ERROR_EXIT_CODE);
        }
    } else if (strcmp(argv[3], "LIB") == 0) {

        io_type = STD_IO;

        if((sys_file_descriptor = open(argv[1], O_RDWR)) == NULL) {
            printf("The file %s could not have been opened\n", argv[1]);
            exit(ERROR_EXIT_CODE);
        }
    } else {
        usage();
        exit(ERROR_EXIT_CODE);
    }

    return 0;
}