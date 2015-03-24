#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#define NULL ((void *)0)


void verify_not_null(void * v)
{
    if(v == NULL)
    {
        printf("Memory allocation error. Exiting now.\n");
        exit(1);
    }
}