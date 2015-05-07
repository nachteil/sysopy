#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#include "time.h"


void error(char *);
void mem_error();
void zero(char *, int);
char *format_hour(struct tm*);
char *get_formatted_hour();

#endif //SYSOPY_COMMON_H
