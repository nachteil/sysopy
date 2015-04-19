#define _GNU_SOURCE

#include "rdfifo.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "errno.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "stdlib.h"
#include "unistd.h"
#include "limits.h"
#include "common.h"

int main(int argc, char * argv []) {

    if(argc != 2) {
        printf("One program argument expected - FIFO file name\n");
        exit(1);
    }

    int fifo_fd;
    char *read_buf;

    // create fifo with read/write permission for the user
    if(mkfifo(argv[1], S_IRUSR | S_IWUSR) < 0) {
        error("FIFO creation error");
    }

    printf("Opening fifo...\n");

    if((fifo_fd = open(argv[1], O_RDONLY)) < 0) {
        error("FIFO open error");
    }

    printf("FIFO opened\n");

    if((read_buf = (char*) malloc(PIPE_BUF)) == NULL) {
        mem_error();
    }

    printf("Entering message loop...\n");

    char * pid = malloc(7);
    char * write_date = malloc(9);
    char * msg = malloc(PIPE_BUF - 21);

    if(!pid || !write_date || !msg) {
        mem_error();
    }

    while(read(fifo_fd, read_buf, PIPE_BUF)) {

        extract_pid(read_buf, pid);
        extract_write_date(read_buf, write_date);
        extract_msg(read_buf, msg);

        char *read_hour = get_formatted_hour();

        printf("%s - %s - %s - %s", read_hour, pid, write_date, msg);
        zero(read_buf, PIPE_BUF);
    }

    free(pid);
    free(write_date);
    free(read_buf);
    free(msg);

    if(remove(argv[1]) < 0) {
        error("FIFO remove error");
    }

    exit(0);
}

void extract_pid(char *msg, char *out) {

    out[6] = (char) 0;
    int i;
    for(i = 0; i < 6; ++i) {
        out[i] = msg[i];
    }
}

void extract_write_date(char *msg, char *out) {
    out[8] = 0;
    int i;
    for(i = 0; i < 8; ++i) {
        out[i] = msg[i+9];
    }
}

void extract_msg(char *msg, char *out) {

    int i = 0;
    while((out[i] = msg[(i)+20]) != (char)0) ++i;

}