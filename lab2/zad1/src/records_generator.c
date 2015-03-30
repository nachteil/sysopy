#include "records_generator.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define ERROR_EXIT_CODE 1
#define EXPECTED_NUMBER_OF_ARGUMENTS 3

// TODO: zliczyc liczbe przestawien

void usage() {

    printf("Usage:\n\trecords_generator FNAME REC_SIZE NUM_REC\n");
    printf("\t\tFNAME - name of file to generate records to\n");
    printf("\t\tREC_SIZE - size of a single record in bytes\n");
    printf("\t\tNUM_REC - number of records to generate\n");

}

void write_records(FILE *file_handler, int rec_size, int num_rec) {

    if(file_handler == NULL) { // TODO: this should write to STDERR
        print_error_message("Null file handler received. Ending program.");
        exit(ERROR_EXIT_CODE);
    }

    srand((unsigned int) time(NULL));

    for(int i = 0; i < num_rec; ++i) {
        for(int j = 0; j < rec_size; ++j) {
            char c = (char) (rand() % 256);
            fwrite(&c, 1, 1, file_handler);
        }
    }
}

void print_error_message(const char const * msg) {
    printf("%s\n", msg);
}

int main(int argc, char * argv[])
{
    if( argc != EXPECTED_NUMBER_OF_ARGUMENTS + 1 ) {
        usage();
        return ERROR_EXIT_CODE;
    }

    char * fname = argv[1];
    int rec_size = atoi(argv[2]);
    int num_rec = atoi(argv[3]);

    FILE *file_handler;

    if((file_handler = fopen(fname, "wb")) == NULL) {
        print_error_message("The file could not have been opened");
        exit(ERROR_EXIT_CODE);
    }

    write_records(file_handler, rec_size, num_rec);

    fclose(file_handler);
    return 0;
}