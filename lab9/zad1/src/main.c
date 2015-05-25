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
pthread_mutex_t fork_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
pthread_cond_t fork_cv[NUM_PHILOSOPHERS];

int main(int argc, char *argv[]) {

    for(int i = 0; i < NUM_PHILOSOPHERS; ++i) {
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

    printf("Philosopher %d starts\n", my_id);

    for(int count = 0; count < NUM_COURSES_TO_EAT; ++count) {

        pthread_cond_t first_cv = fork_cv[first_fork_id];
        pthread_cond_t second_cv = fork_cv[second_fork_id];

        if (pthread_mutex_lock(&fork_mutex) != 0) {
            error("Error acquiring fork mutex");
        }

        printf("Philosopher %d passed mutex\n", my_id);

        while(pthread_cond_wait(&first_cv, &fork_mutex) != 0);
        printf("Philosopher %d got fork %d\n", my_id, first_fork_id);
        while(pthread_cond_wait(&second_cv, &fork_mutex) != 0);

        printf("Philosopher %d acquired forks %d and %d. Started eating.\n", my_id, first_fork_id, second_fork_id);

        int MICROS_PER_MILLI = 1000;
        usleep(10 * MICROS_PER_MILLI);

        if(pthread_mutex_unlock(&fork_mutex) != 0) {error("Mutex unlock failed");}
        if(pthread_mutex_unlock(&fork_mutex) != 0) {error("Mutex unlock failed");}

        if(pthread_cond_broadcast(&first_cv) != 0) {error("CV broadcast failed");}
        if(pthread_cond_broadcast(&second_cv) != 0) {error("CV broadcast failed");}
    }

    return NULL;
}