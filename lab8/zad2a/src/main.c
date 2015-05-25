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
#include "signal.h"

#define RECORD_SIZE 1024
#define NUM_REQUIRED_ARGS 6

void usage();
void cleanup_fun();

pthread_t *thread_ids;
pthread_mutex_t read_protect_mutex = PTHREAD_MUTEX_INITIALIZER;

int ALL_THREADS_RUNNING;

int testcase_num;
int sig_num;

int main(int argc, char * argv[]) {

    if(argc != NUM_REQUIRED_ARGS + 1) {
        usage();
    }

    char * filename = argv[2];
    int filedes;
    if((filedes = open(filename, O_RDONLY)) == -1) {
        error("Cannot open file");
    }

    testcase_num = atoi(argv[5]);
    sig_num = get_signum(argv[6]);

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

    switch(testcase_num) {

        case 1:
            if(kill(getpid(), sig_num) != 0) {
                error("kill() failed");
            }
            break;

        case 2:
            if(signal(sig_num, SIG_IGN) == SIG_ERR) {
                error("Error setting SIG_IGN");
            }
            if(kill(getpid(), sig_num) != 0) {
                error("kill() failed");
            }
            break;

        case 3:

            if (signal(sig_num, sig_handler) == SIG_ERR) {
                error("Error setting SIGUSR1 handler");
            }
            if(kill(getpid(), sig_num) != 0) {
                error("kill() failed");
            }
            break;

        case 4:

            // choose random - all child threads have this signal blocked
            if (pthread_kill(thread_ids[rand() % ctrl.num_threads], sig_num) != 0) {
                error("Error sending pthread_kill");
            }
            break;

        case 5:

            if (pthread_kill(thread_ids[rand() % ctrl.num_threads], sig_num) != 0) {
             error("On pthread_kill()");
            }
            break;

        default:
            error("Wrong test number");
    }

    for(int i = 0; i < ctrl.num_threads; ++i) {
        if(pthread_join(thread_ids[i], NULL) !=0) {
            error("join() failed");
        }
    }

    return 0;
}

void *thread_work_function(void *arg_ptr) {

    if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) {
        error("pthread_setcancelstate() error");
    }

    if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0) {
        error("pthread_setcanceltype() error");
    }


    if(testcase_num == 3 || testcase_num == 5) {
        if(signal(SIGUSR1, sig_handler) == SIG_ERR){
            error("Error setting sig_handler for SIGUSR1");
        }
        if(signal(SIGTERM, sig_handler) == SIG_ERR) {
            error("Error setting sig_handler for SIGTERM");
        }
    } else if(testcase_num == 4){
        signal(sig_num, SIG_IGN);
    }

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

    while(1) {

        // read operation - guarded by critical section
        if(pthread_mutex_lock (&read_protect_mutex) != 0) {
            error("Mutex locking error");
        }
        int read_bytes;
        char *rec_buff = (char*)pthread_getspecific(key);
        if(rec_buff == NULL) {
            error("pthread_getspecific() returned NULL");
        }
        if((read_bytes = read(ctrl.filedes, rec_buff, buff_size)) < 0) {
            error("File reading error");
        }

        if(read_bytes < ctrl.num_records_to_read * RECORD_SIZE) {
            lseek(ctrl.filedes, 0, SEEK_SET);
        }

        if(pthread_mutex_unlock (&read_protect_mutex) != 0) {
            error("Mutex unlocking error");
        }


        for(int i = 0; i < ctrl.num_records_to_read; ++i) {

            strncpy(tmp_buff, &rec_buff[i*RECORD_SIZE + sizeof(int)], RECORD_SIZE - sizeof(int));
            tmp_buff[RECORD_SIZE - sizeof(int)] = (char)0;
            if(strstr(tmp_buff, ctrl.word) != NULL) {
                // do nothing
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

    printf("Usage: threaded_finder N filename M word test sig\n");
    printf("\tN        - number of threads to be created\n");
    printf("\tfilename - name of file to read from\n");
    printf("\tM        - number of records read at once\n");
    printf("\tword     - word to look for in records\n");
    printf("\ttest      - test case number\n");
    printf("\tsig      - signal name\n");

    exit(1);
}

void cleanup_fun(pthread_key_t *key_ptr) {
    char *buf_ptr = (char *) pthread_getspecific(*key_ptr);
    free(buf_ptr);
}

void sig_handler(int sig_no) {
    if(sig_no == SIGUSR1) {
        printf("SIGUSR1 received\n");
    } else if(sig_no == SIGTERM) {
        printf("SIGTERM received\n");
    }
    printf("PID: %d TID: %d\n", getpid(), (int) pthread_self());
}

int get_signum(char *arg) {

    if(!strcmp(arg, "SIGUSR1"))
        return SIGUSR1;
    if(!strcmp(arg, "SIGTERM"))
        return SIGTERM;
    if(!strcmp(arg, "SIGKILL"))
        return SIGKILL;
    if(!strcmp(arg, "SIGSTOP"))
        return SIGSTOP;

    error("Unrecognized signal");
    return -1;
}