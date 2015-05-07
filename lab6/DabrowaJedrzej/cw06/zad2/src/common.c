#include "common.h"
#include "stdlib.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "signal.h"

void error(char *str) {

    printf("%s, errno %d\n", str, errno);
    printf("%s\n", strerror(errno));

    exit(errno);
}

void reset_line(char *buff, int n) {
    for (int i = 0; i < n; ++i) {
        buff[i] = 0;
    }
}

void mem_error() {
    error("Memory allocation error");
}

void zero(char *str, int n) {
    int i;
    for (i = 0; i < n; ++i) {
        str[i] = (char) 0;
    }
}

int spawn_thread(int (*fn)(void *), void *argptr) {

    int stack_size = 4096;
    void *child_stack = (void *) malloc(stack_size);
    if (child_stack == NULL) {
        mem_error();
    }
    child_stack += stack_size;

    int thread_id;
    if ((thread_id = clone(fn, child_stack, CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_VM, argptr)) <
        0) {
        error("Couldn't create thread");
    }
    return thread_id;
}

char *format_hour(struct tm *info) {

    char *formatted = (char *) malloc(9);
    if (formatted == NULL) {
        mem_error();
    }

    formatted[2] = formatted[5] = ':';
    formatted[8] = (char) 0;

    formatted[0] = '0' + (info->tm_hour) / 10;
    formatted[1] = '0' + (info->tm_hour) % 10;
    formatted[3] = '0' + (info->tm_min) / 10;
    formatted[4] = '0' + (info->tm_min) % 10;
    formatted[6] = '0' + (info->tm_sec) / 10;
    formatted[7] = '0' + (info->tm_sec) % 10;

    return formatted;
}

char *get_formatted_hour() {
    time_t t = time(NULL);
    struct tm *time_info = localtime(&t);
    return format_hour(time_info);
}