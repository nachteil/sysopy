#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H
#define _GNU_SOURCE

#include "time.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sched.h"

void error(char *);
void mem_error();
void zero(char *, int);
char *format_hour(struct tm*);
char *get_formatted_hour();
int spawn_thread(int (*fn)(void *), void *);
void reset_line(char *, int);

#endif //SYSOPY_COMMON_H
