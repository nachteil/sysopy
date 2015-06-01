#define _GNU_SOURCE

#include "rw.h"
#include "common.h"
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"

int library[LIBRARY_SIZE];

pthread_t reader_threads[NUM_READERS];
pthread_t *writer_threads;

sem_t sem_writer;
sem_t sem_library_empty;

int num_writer_threads;

int main(int argc, char *argv[]) {

    if (argc != EXPECTED_NUM_PARAMS+1) {
        printf("Usage: rw <n>\n");
        printf("\tn - number of writer_threads");
        exit(0);
    }

    num_writer_threads = atoi(argv[1]);

    srand(time(NULL));

    writer_threads = (pthread_t *) malloc(num_writer_threads * sizeof(pthread_t));
    if (writer_threads == NULL) {
        mem_error();
        exit(1);
    }

    for (int i = 0; i < LIBRARY_SIZE; ++i) {
        library[i] = 0;
    }

    init_semaphore(&sem_writer, 1);
    init_semaphore(&sem_library_empty, NUM_READERS);

    for (int i = 0; i < NUM_READERS; ++i) {
        int reader_task = (rand() % MAX_NUM)+1;
        if (pthread_create(&(reader_threads[i]), NULL, reader_fun, (void *) ((long)reader_task)) != 0) {
            error("pthread_create() error");
        }
    }

    for (int i = 0; i < num_writer_threads; ++i) {
        if (pthread_create(&(writer_threads[i]), NULL, writer_fun, NULL) != 0) {
            error("pthread_create() error");
        }
    }

    for (int i = 0; i < num_writer_threads; ++i) {
        if (pthread_join(writer_threads[i], NULL) != 0) {
            error("pthread_join() error");
        }
    }

    for (int i = 0; i < NUM_READERS; ++i) {
        if (pthread_join(reader_threads[i], NULL) != 0) {
            error("pthread_join() error");
        }
    }

    return 0;
}

void init_semaphore(sem_t *sem, int n) {
    if (sem_init(sem, 0, n) != 0) {
        error("sem_init() error");
    }
}

void wait(sem_t *sem) {
    if (sem_wait(sem) != 0) {
        error("sem_wait() error");
    }
}

void post(sem_t *sem) {
    if (sem_post(sem) != 0) {
        error("sem_post() error");
    }
}

int reader_read(int num) {

    int num_found = 0;
    for (int i = 0; i < LIBRARY_SIZE; ++i) {
        if (library[i] == num) {
            num_found++;
        }
    }
    return num_found;
}

int writer_write() {

    int num_indices_to_modify = 1+(rand() % 100);

    for (int i = 0; i < num_indices_to_modify; ++i) {
        int index_to_modify = rand() % LIBRARY_SIZE;
        library[index_to_modify] = rand() % (MAX_NUM+1);
    }
    return num_indices_to_modify;
}

void *reader_fun(void *arg_ptr) {

    pthread_t pthread_id = pthread_self();
    int my_id;

    for(int i = 0; i < NUM_READERS; ++i) {
        if(pthread_equal(pthread_id, reader_threads[i])) {
            my_id = i;
            break;
        }
    }

    int num = (int) ((long)arg_ptr);
    printf("Looking for: %d\n", num);

    while (1 > 0) {

        usleep(get_rand_usec());

        wait(&sem_library_empty);
        int num_found = reader_read(num);
        printf("Reader %d - found %d occurences of %d\n", my_id+1, num_found, num);

        post(&sem_library_empty);
    }

    return NULL;
}

void *writer_fun(void *arg_ptr) {

    pthread_t pthread_id = pthread_self();
    int my_id;

    for(int i = 0; i < num_writer_threads; ++i) {
        if(pthread_equal(pthread_id, writer_threads[i])) {
            my_id = i;
            break;
        }
    }

    while (1 > 0) {

        usleep(get_rand_usec());
        wait(&sem_writer);

        // assure there are no readers in library
        for (int i = 0; i < NUM_READERS; ++i) {
            wait(&sem_library_empty);
        }
        int modified_indices = writer_write();
        printf("Writer %d - %d indices modified\n", my_id+1, modified_indices);

        for (int i = 0; i < NUM_READERS; ++i) {
            post(&sem_library_empty);
        }
        post(&sem_writer);
    }

    return NULL;
}

unsigned int get_rand_usec() {

    double d = rand();
    d /= RAND_MAX;
    return  (unsigned int) (d * 5000000 + 1000000);
}


