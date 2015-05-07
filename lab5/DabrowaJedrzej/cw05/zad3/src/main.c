#define _GNU_SOURCE

#include "main.h"
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

#define MAXLINE 1024

int main(int argc, char * argv []) {

    FILE *from_ls;
    FILE *to_grep;

    char *buffer;

    from_ls = popen("ls -l", "r");
    to_grep = popen("grep ^d > folders.txt", "w");

    buffer = (char *) malloc(MAXLINE);

    if( !from_ls || !to_grep) {
        error("POPEN error");
    }

    if(!buffer) {
        mem_error();
    }

    while(fgets(buffer, MAXLINE, from_ls)) {
        fputs(buffer, to_grep);
    }

    if(pclose(from_ls) < 0 || pclose(to_grep) < 0) {
        error("PCLOSE error");
    }

    exit(0);
}