#define _GNU_SOURCE

#include "main.h"
#include <fcntl.h>
#include "stdlib.h"
#include <pthread.h>
#include "common.h"
#include <sys/types.h>
#include "sys/stat.h"
#include <unistd.h>
#include "stdio.h"
#include "string.h"

#define NUM_PHILOSOPHERS 5
#define NUM_COURSES_TO_EAT 10

pthread_t philosopher_threads[NUM_PHILOSOPHERS];
pthread_mutex_t fork_mutex[NUM_PHILOSOPHERS];
pthread_cond_t fork_cv[NUM_PHILOSOPHERS];

int main(int argc, char *argv[]) {

    for(int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        fork_mutexes[i] = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
        fork_cv[i] = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    }

    int ints[NUM_PHILOSOPHERS];
    for(int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        ints[i] = i+1;
    }

    for(int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        if (pthread_create(&philosopher_threads[i], NULL, dining_function, &(ints[i])) != 0) {
            error("Thread creation error");
        }
    }

    for(int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        if (pthread_join(philosopher_threads[i], NULL) != 0) {
            error("Thread join error");
        }
    }

    exit(0);
}

void *dining_function(void *id_ptr) {

    int my_id = *((int *) id_ptr);

    int first_fork_id = (my_id+1) % NUM_PHILOSOPHERS;;
    int second_fork_id = my_id;

    if(my_id %2) {
        first_fork_id = my_id;
        second_fork_id = (my_id+1) % NUM_PHILOSOPHERS;
    }

    for(int count = 0; count < NUM_COURSES_TO_EAT; ++count) {

        pthread_mutex_t first_fork = fork_mutexes[first_fork_id];
        pthread_mutex_t second_fork = fork_mutexes[second_fork_id];

        pthread_cond_t first_cv = fork_cv[first_fork_id];
        pthread_cond_t second_cv = fork_cv[second_fork_id];

        if (pthread_mutex_lock(&first_fork) != 0) {
            error("Error acquiring fork (mutex lock operation)");
        }
        while(pthread_cond_wait(&got_request, &a_mutex) != 0) {

        }
        if (pthread_mutex_lock(&second_fork) != 0) {
            error("Error acquiring fork (mutex lock operation)");
        }

        printf("Philosopher %d acquired forks %d and %d. Started eating.\n", my_id, first_fork_id, second_fork_id);

        int MICROS_PER_MILLI = 1000;
        usleep(10 * MICROS_PER_MILLI);

        if (pthread_mutex_unlock(&first_fork) != 0) {
            error("Error releasing fork (mutex lock operation)");
        }
        if (pthread_mutex_unlock(&second_fork) != 0) {
            error("Error releasing fork (mutex lock operation)");
        }
    }

    return NULL;
}