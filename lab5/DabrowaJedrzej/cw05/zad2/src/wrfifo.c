#define _GNU_SOURCE

#include "wrfifo.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "errno.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "limits.h"
#include "common.h"
#include "time.h"

int main(int argc, char * argv[]) {

    if(argc != 2) {
        printf("One program argument expected - FIFO file name\n");
        exit(1);
    }

    int fifo_fd;
    char *write_buf;
    char *read_buf;
    int read_buf_size = PIPE_BUF - 21;

    if((fifo_fd = open(argv[1], O_WRONLY)) < 0) {
        error("FIFO open error");
    }

    if((write_buf = (char*) malloc(PIPE_BUF)) == NULL) {
        mem_error();
    }

    if((read_buf = (char*) malloc(read_buf_size)) == NULL) {
        mem_error();
    }

    printf("FIFO opened, enter message loop...\n");

    while(fgets(read_buf, read_buf_size, stdin)) {

        char *write_hour = get_formatted_hour();
        sprintf(write_buf, "%6d - %s - %s", getpid(), write_hour, read_buf);
        write(fifo_fd, write_buf, strlen(write_buf));
        free(write_hour);
    }

    exit(0);
}
