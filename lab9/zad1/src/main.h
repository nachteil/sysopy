#ifndef SYSOPY_MAIN_H
#define SYSOPY_MAIN_H

#include "pthread.h"

void *dining_function(void *);

void signal_condition(pthread_cond_t*);
void unlock_mutex(pthread_mutex_t*);
void wait_condition(pthread_cond_t*, pthread_mutex_t*);
void lock_mutex(pthread_mutex_t*);

#endif //SYSOPY_MAIN_H
