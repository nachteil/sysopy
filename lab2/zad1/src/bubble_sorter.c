#include "stdio.h"
#include "sys/types.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "fcntl.h"
#include "bubble_sorter.h"
#include "bubble_sort.h"
#include <sys/stat.h>
#include <sys/times.h>

#define EXPECTED_ARGUMENTS_COUNT 3
#define ERROR_EXIT_CODE 1

#define SYS_IO 3534533
#define STD_IO 88345

int io_type;

FILE * stdio_file_handler = NULL;
int sys_file_descriptor = -1;

int bytes_per_record;
int number_of_records;

void usage() {
    printf("USAGE:\n\tbubble_sorter RECF_PATH RECLEN TYPE\n");
    printf("\tRECF_PATH - path to a file containing records to sort\n");
    printf("\tRECLEN - size (in bytes) of a single record\n");
    printf("\tTYPE - in/out functions to use in program:\n");
    printf("\t\tSYS - using system io\n");
    printf("\t\tLIB - using library io\n");
}


void swap_records_sys_io(int a, int b) {

    long current_file_position = lseek(sys_file_descriptor, 0, SEEK_CUR);
    if(current_file_position == -1) {
        printf("File access error.\n");
        exit(1);
    }

    long a_start_index = a * bytes_per_record;
    long b_start_index = b * bytes_per_record;

    void * a_buffer = malloc(bytes_per_record);
    void * tmp_buffer = malloc(bytes_per_record);

    if( !a_buffer || !tmp_buffer) {
        printf("Memory allocation error.\n");
        exit(1);
    }

    // read a into buffer
    if(lseek(sys_file_descriptor, a_start_index, SEEK_SET) == -1) {
        printf("File access error.\n");
        exit(1);
    }
    if(read(sys_file_descriptor, a_buffer, bytes_per_record) == -1) {
        printf("File read error.\n");
        exit(1);
    }

    // read b into buffer
    if(lseek(sys_file_descriptor, b_start_index, SEEK_SET) == -1) {
        printf("File access error.\n");
        exit(1);
    }
    if(read(sys_file_descriptor, tmp_buffer, bytes_per_record) == -1) {
        printf("File read error.\n");
        exit(1);
    }

    // write contents of b into a
    if(lseek(sys_file_descriptor, a_start_index, SEEK_SET) == -1) {
        printf("File access error.\n");
        exit(1);
    }
    if(write(sys_file_descriptor, tmp_buffer, bytes_per_record) == -1) {
        printf("File write error.\n");
        exit(1);
    }

    // write contest of a into b
    if(lseek(sys_file_descriptor, b_start_index, SEEK_SET) == -1) {
        printf("File access error.\n");
        exit(1);
    }
    if(write(sys_file_descriptor, a_buffer, bytes_per_record) == -1) {
        printf("File write error.\n");
        exit(1);
    }

    free(a_buffer);
    free(tmp_buffer);

    // restore previous file position
    if(lseek(sys_file_descriptor, current_file_position, SEEK_SET) != current_file_position) {
        printf("File access error.\n");
        exit(1);
    }

}

void swap_records_std_io(int a, int b) {

    long current_file_position = ftell(stdio_file_handler);
    if(current_file_position == -1) {
        printf("File access error 1.\n");
        exit(1);
    }

    long a_start_index = a * bytes_per_record;
    long b_start_index = b * bytes_per_record;

    void * a_buffer = malloc(bytes_per_record);
    void * tmp_buffer = malloc(bytes_per_record);

    if( !a_buffer || !tmp_buffer) {
        printf("Memory allocation error.\n");
        exit(1);
    }

    // read a into buffer
    if(fseek(stdio_file_handler, a_start_index, SEEK_SET) != 0) {
        printf("File access error 2.\n");
        exit(1);
    }
    if(fread(a_buffer, bytes_per_record, 1, stdio_file_handler) != 1) {
        printf("File read error 3.\n");
        exit(1);
    }

    // read b into buffer
    if(fseek(stdio_file_handler, b_start_index, SEEK_SET) == -1) {
        printf("File access error 4.\n");
        exit(1);
    }
    if(fread(tmp_buffer, bytes_per_record, 1, stdio_file_handler) != 1) {
        printf("File read error 5.\n");
        exit(1);
    }

    // write contents of b into a
    if(fseek(stdio_file_handler, a_start_index, SEEK_SET) == -1) {
        printf("File access error 6.\n");
        exit(1);
    }
    if(fwrite(tmp_buffer, bytes_per_record, 1, stdio_file_handler) == -1) {
        printf("File write error.\n");
        exit(1);
    }

    // write contest of a into b
    if(fseek(stdio_file_handler, b_start_index, SEEK_SET) == -1) {
        printf("File access error 7.\n");
        exit(1);
    }
    if(fwrite(a_buffer, bytes_per_record, 1, stdio_file_handler) == -1) {
        printf("File write error.\n");
        exit(1);
    }

    free(a_buffer);
    free(tmp_buffer);

    // restore previous file position
    if(fseek(stdio_file_handler, current_file_position, SEEK_SET) != 0) {
        printf("File access error 8.\n");
        exit(1);
    }
}

void set_file_start_std_io() {
    if(fseek(stdio_file_handler, 0, SEEK_SET) != 0) {
        printf("File access error.\n");
        exit(1);
    }
}

void set_file_start_sys_io() {

    if(lseek(sys_file_descriptor, 0, SEEK_SET) == -1)
    {
        printf("File access error.\n");
        exit(1);
    }
    return;
}


void * get_next_record_sys_io()
{

    void *buffer = malloc(bytes_per_record);
    if(buffer == NULL)
    {
        printf("Memory allocation error.\n");
        exit(1);
    }

    int bytes_read = read(sys_file_descriptor, buffer, bytes_per_record);
    if(bytes_read == 0) {
        printf("End of records\n");
        if(lseek(sys_file_descriptor, 0, SEEK_SET) == -1)
        {
            printf("File access error.\n");
            exit(1);
        }
        printf("Start from the beggining\n");
        return get_next_record_sys_io();
    } else if(bytes_read != bytes_per_record)
    {
        printf("Reading error.\n");
        exit(1);
    }

    return buffer;
}

void * get_next_record_std_io()
{
    void *buffer = malloc(bytes_per_record);
    if(buffer == NULL)
    {
        printf("Memory allocation error.\n");
        exit(1);
    }

    int bytes_read = fread(buffer, bytes_per_record, 1, stdio_file_handler);

    if(bytes_read != 1 || ferror(stdio_file_handler))
    {
        printf("File reading error 1.\n");
        exit(1);
    }

    if(feof(stdio_file_handler))
    {
        if(fseek(stdio_file_handler, 0, SEEK_SET) != 0) {
            printf("File access error.\n");
            exit(1);
        }
    }

    return buffer;
}

// args: path_to_file, length_of_record_in_bytes
int main(int argc, char * argv[])
{
    if(argc != EXPECTED_ARGUMENTS_COUNT + 1) {
        usage();
        exit(ERROR_EXIT_CODE);
    }

    if( access( argv[1], F_OK ) == -1 ) {
        printf("File: %s does not exist\n", argv[1]);
    }

    bytes_per_record = atoi(argv[2]);

    if (strcmp(argv[3], "SYS") == 0) {

        io_type = SYS_IO;

        if((sys_file_descriptor = open(argv[1], O_RDWR)) == -1) {
            printf("The file %s could not have been opened\n", argv[1]);
            exit(ERROR_EXIT_CODE);
        }

        struct stat st;
        fstat(sys_file_descriptor, &st);
        if(st.st_size % bytes_per_record) {
            printf("%s file's length and size of the record do not match.\n", argv[1]);
            exit(1);
        }
        number_of_records = st.st_size / bytes_per_record;

    } else if (strcmp(argv[3], "LIB") == 0) {

        io_type = STD_IO;

        if((stdio_file_handler = fopen(argv[1], "r+b")) == NULL) {
            printf("The file %s could not have been opened\n", argv[1]);
            exit(ERROR_EXIT_CODE);
        }

        struct stat st;
        stat(argv[1], &st);
        if(st.st_size % bytes_per_record) {
            printf("%s file's length and size of the record do not match.\n", argv[1]);
            exit(1);
        }
        number_of_records = st.st_size / bytes_per_record;

    } else {
        usage();
        exit(ERROR_EXIT_CODE);
    }

    struct tms *buf = malloc(sizeof(struct tms));
    if(buf == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    long ticks = sysconf(_SC_CLK_TCK);

    printf("\nRunning configuration\n\tRecord size: %d B\n\tNumber of records: %d\n\tFile operations library: %s\n",
        bytes_per_record, number_of_records, io_type == SYS_IO ? "Unix IO" : "C's STDIO"
    );

    if(io_type == STD_IO) {
        perform_bubble_sort(&get_next_record_std_io, &swap_records_std_io, &set_file_start_std_io, bytes_per_record, number_of_records);
    } else {
        perform_bubble_sort(&get_next_record_sys_io, &swap_records_sys_io, &set_file_start_sys_io, bytes_per_record, number_of_records);
    }

    times(buf);
    printf("\nResults:\n\tUser time: %5.3lf s, Sys time: %5.3lf s\n", ((double) buf -> tms_utime) / ticks, ((double) buf -> tms_stime) / ticks);
    printf("\n* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \n\n");

    return 0;
}