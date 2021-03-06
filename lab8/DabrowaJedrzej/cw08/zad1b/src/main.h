#ifndef SYSOPY_MAIN_H
#define SYSOPY_MAIN_H

#include <pthread.h>

typedef struct ctrl_struct {
    int num_threads;
    int filedes;
    int num_records_to_read;
    char *word;
    pthread_key_t key;
} ctrl_struct;

void *thread_work_function(void *);
void init_buf(pthread_key_t *, int);

#endif //SYSOPY_MAIN_H
