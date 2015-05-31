#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#include "time.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sched.h"

#define MSG_MAX_LEN 256

void error(char *);
void mem_error();
void zero(char *, int);
char *format_hour(struct tm*);
char *get_formatted_hour();
void reset_line(char *, int);
char *get_timestamp_with_millis();
void init_mem(char **, int);
char *get_copy(char *);

#endif //SYSOPY_COMMON_H
