#ifndef SYSOPY_MAIN_H
#define SYSOPY_MAIN_H

#include "semaphore.h"

#define EXPECTED_NUM_PARAMS 1
#define LIBRARY_SIZE 5000
#define MAX_NUM 999
#define NUM_READERS 15

unsigned int get_rand_usec();

void init_semaphore(sem_t *, int);
void wait(sem_t *);
void post(sem_t *);

int reader_read(int);
int writer_write();

void *reader_fun(void *);
void *writer_fun(void *);

#endif //SYSOPY_MAIN_H
