#define _GNU_SOURCE

#include "main.h"
#include "common.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "stdio.h"
#include "time.h"
#include "sched.h"

#define RECORD_SIZE 1024
#define NUM_REQUIRED_ARGS 4

void usage();
void cleanup_fun();

pthread_t *thread_ids;
pthread_mutex_t read_protect_mutex = PTHREAD_MUTEX_INITIALIZER;

int ALL_THREADS_RUNNING;

int main(int argc, char * argv[]) {

    if(argc != NUM_REQUIRED_ARGS + 1) {
        usage();
    }

    char * filename = argv[2];
    int filedes;
    if((filedes = open(filename, O_RDONLY)) == -1) {
        error("Cannot open file");
    }

    ctrl_struct ctrl;
    ctrl.num_threads = atoi(argv[1]);
    ctrl.filedes = filedes;
    ctrl.num_records_to_read = atoi(argv[3]);
    ctrl.word = argv[4];

    if((thread_ids = (pthread_t *) malloc(ctrl.num_threads * sizeof(pthread_t))) == NULL) {
        mem_error();
    }

    pthread_key_t rec_buffer_key;
    if((pthread_key_create(&rec_buffer_key, cleanup_fun)) != 0) {
        error("Cannot create key");
    }
    ctrl.key = rec_buffer_key;

    ALL_THREADS_RUNNING = 0;
    for(int i = 0; i < ctrl.num_threads; ++i) {
        if((pthread_create(&(thread_ids[i]), NULL, thread_work_function, &ctrl)) != 0) {
            error("Thread creation error");
        }
    }
    ALL_THREADS_RUNNING = 1;

    for(int i = 0; i < ctrl.num_threads; ++i) {
        pthread_join(thread_ids[i], NULL);
    }

    return 0;
}

void *thread_work_function(void *arg_ptr) {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    ctrl_struct ctrl = *((ctrl_struct *)arg_ptr);

    // wait for other threads to start
    // this is to avoid cancelling not yet started threads in case of early success
    int USECS_PER_MILLISEC = 1000;
    while(!ALL_THREADS_RUNNING) {
        usleep(50*USECS_PER_MILLISEC);
    }

    pthread_key_t key = ctrl.key;
    char *tmp_buff;
    int buff_size = ctrl.num_records_to_read * RECORD_SIZE;

    init_buf(&key, buff_size);

    if((tmp_buff = (char *) malloc(RECORD_SIZE)) == NULL) {
        mem_error();
    }

    int should_loop = 1;

    while(should_loop) {

        // read operation - guarded by critical section
        pthread_mutex_lock (&read_protect_mutex);
        int read_bytes;
        char *rec_buff = (char*)pthread_getspecific(key);
        if((read_bytes = read(ctrl.filedes, rec_buff, buff_size)) < 0) {
            error("File reading error");
        }
        pthread_mutex_unlock (&read_protect_mutex);

        pthread_testcancel();

        if(read_bytes < ctrl.num_records_to_read * RECORD_SIZE) {
            should_loop = 0;
        }

        for(int i = 0; i < ctrl.num_records_to_read; ++i) {

            strncpy(tmp_buff, &rec_buff[i*RECORD_SIZE + sizeof(int)], RECORD_SIZE - sizeof(int));
            tmp_buff[RECORD_SIZE - sizeof(int)] = (char)0;
            int rec_id = *((int *) &(rec_buff[i*RECORD_SIZE]));

            if(strstr(tmp_buff, ctrl.word) != NULL) {
                printf("Found by thread: %u, in record: %d\n", (unsigned int) pthread_self(), rec_id);
                should_loop = 0;

                for(int j = 0; j < ctrl.num_threads; ++j) {
                    if(!pthread_equal(thread_ids[j], pthread_self())) {
                        pthread_cancel(thread_ids[j]);
                    }
                }

            }
        }
    }
    return NULL;
}

void init_buf(pthread_key_t *key, int size) {

    char *rec_buff;

    if((rec_buff = (char *) malloc(size)) == NULL) {
        mem_error();
    }

    if(pthread_setspecific(*key, (void *)rec_buff) != 0) {
        error("pthread_setspecific() error");
    }
}

void usage() {

    printf("Usage: threaded_finder N filename M word\n");
    printf("\tN        - number of threads to be created\n");
    printf("\tfilename - name of file to read from\n");
    printf("\tM        - number of records read at once\n");
    printf("\tword     - word to look for in records\n");

    exit(1);
}

void cleanup_fun(pthread_key_t *key_ptr) {
    char *buf_ptr = (char *) pthread_getspecific(*key_ptr);
    free(buf_ptr);
}