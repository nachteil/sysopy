#include "common.h"
#include "stdlib.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"

void error(char *str) {

    printf("%s\n", str);
    exit(-1);
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
        error("Memory allocation error");
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