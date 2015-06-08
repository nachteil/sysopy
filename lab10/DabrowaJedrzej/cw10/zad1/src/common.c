#include "common.h"
#include "stdlib.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "signal.h"
#include "sys/time.h"

void error(char *str) {

    printf("%s, errno %d\n", str, errno);
    printf("%s\n", strerror(errno));

    exit(errno);
}

void reset_line(char * buff, int n) {
    for(int i = 0; i < n; ++i) {
        buff[i] = 0;
    }
}

void mem_error() {
    error("Memory allocation error");
}

void zero(char *str, int n) {
    int i;
    for(i = 0; i < n; ++i) {
        str[i] = (char) 0;
    }
}


char *format_hour(struct tm* info) {

    char *formatted = (char *) malloc(9);
    if(formatted == NULL) {
        mem_error();
    }

    formatted[2] = formatted[5] = ':';
    formatted[8] = (char) 0;

    formatted[0] = '0' + (info -> tm_hour) / 10;
    formatted[1] = '0' + (info -> tm_hour) % 10;
    formatted[3] = '0' + (info -> tm_min) / 10;
    formatted[4] = '0' + (info -> tm_min) % 10;
    formatted[6] = '0' + (info -> tm_sec) / 10;
    formatted[7] = '0' + (info -> tm_sec) % 10;

    return formatted;
}

char *get_formatted_hour() {
    time_t t = time(NULL);
    struct tm * time_info = localtime(&t);
    return format_hour(time_info);
}

char *get_timestamp_with_millis() {

    struct timeval tmv;
    gettimeofday(&tmv, NULL);

    time_t current_time_t = tmv.tv_sec;
    struct tm *current_time_tm = localtime(&current_time_t);
    char *no_millis = format_hour(current_time_tm);

    int millis = ((int) tmv.tv_usec) / 1000;


    char *with_millis = (char *) realloc(no_millis, 13);
    with_millis[12] = (char) 0;
    with_millis[11] = '0' + (millis % 10);
    with_millis[10] = '0' + ((millis % 100) / 10 );
    with_millis[9]  = '0' + ((millis & 100) / 100);
    with_millis[8]  = ':';

    return with_millis;
}

void init_mem(char **buf_ptr, int num_bytes) {

    *(buf_ptr) = (char *) malloc(num_bytes);
    if(*buf_ptr == NULL) {
        mem_error();
    }
}

char *get_copy(char *src) {

    int buf_size = strlen(src)+1;
    char *copy = (char *) malloc(buf_size);
    if(copy == NULL) {
        mem_error();
    }
    strcpy(copy, src);

    return copy;
}