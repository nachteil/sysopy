#define _GNU_SOURCE

#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"
#include "semaphore.h"
#include "sched.h"
#include "string.h"
#include "stdlib.h"
#include "signal.h"
#include "sys/time.h"
#include "time.h"
#include "common.h"

#define NUM_PHILOSOPHERS 5
#define COURSES_TO_EAT 10

#define FORK_FREE 63
#define FORK_USED 2345

pthread_cond_t widelec[NUM_PHILOSOPHERS];
pthread_mutex_t fork_mutex[NUM_PHILOSOPHERS];
int fork_status[NUM_PHILOSOPHERS];
pthread_t thread_ids[NUM_PHILOSOPHERS];

int main(int argc, char *argv[]) {
    int ints[NUM_PHILOSOPHERS];
    srand(time(NULL));

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {

        ints[i] = i;
        widelec[i] = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
        fork_mutex[i] = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
        fork_status[i] = FORK_FREE;
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        if (pthread_create(&thread_ids[i], NULL, dining_function, &ints[i])) {
            error("Thread creation error");
        }
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        if (pthread_join(thread_ids[i], NULL)) {
            error("Thread joining error");
        }
    }

    exit(0);
}


void *dining_function(void *arg_ptr) {
    int my_id = *((int *) arg_ptr);
    int MICRO_PER_MILLI = 1000;

    int first_fork = my_id;
    int second_fork = my_id + 1;

    if (my_id == NUM_PHILOSOPHERS - 1) {
        first_fork = 0;
        second_fork = my_id;
    }

    for (int count = 0; count < COURSES_TO_EAT; ++count) {

        usleep(100 * MICRO_PER_MILLI);

        // acquire first fork
        lock_mutex(&fork_mutex[first_fork]);
        while (fork_status[first_fork] == FORK_USED) {
            wait_condition(&widelec[first_fork], &fork_mutex[first_fork]);
        }
        fork_status[first_fork] = FORK_USED;
        unlock_mutex(&fork_mutex[first_fork]);

        // acquire second fork
        lock_mutex(&fork_mutex[second_fork]);
        while (fork_status[second_fork] == FORK_USED) {
            wait_condition(&widelec[second_fork], &fork_mutex[second_fork]);
        }
        fork_status[second_fork] = FORK_USED;
        unlock_mutex(&fork_mutex[second_fork]);

        printf("Philosopher %d is now eating\n", my_id);

        usleep(100 * MICRO_PER_MILLI);

        // release and signal first fork free
        lock_mutex(&fork_mutex[first_fork]);
        fork_status[first_fork] = FORK_FREE;
        signal_condition(&widelec[first_fork]);
        unlock_mutex(&fork_mutex[first_fork]);

        // release and signal second fork free
        lock_mutex(&fork_mutex[second_fork]);
        fork_status[second_fork] = FORK_FREE;
        signal_condition(&widelec[second_fork]);
        unlock_mutex(&fork_mutex[second_fork]);
    }

    printf("%d is done!\n", my_id);
    return NULL;
}

void signal_condition(pthread_cond_t *condition) {
    if (pthread_cond_signal(condition)) { error("Condition signaling error"); }
}

void unlock_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_unlock(mutex) != 0) { error("Mutex unlcoking error"); }
}

void wait_condition(pthread_cond_t *condition, pthread_mutex_t *mutex) {
    if (pthread_cond_wait(condition, mutex) != 0) { error("Condition wait error"); }
}

void lock_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_lock(mutex) != 0) { error("Mutex locking error"); }
}