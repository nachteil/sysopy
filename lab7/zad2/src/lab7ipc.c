#include "lab7ipc.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdio.h"
#include "signal.h"
#include "common.h"
#include "sys/mman.h"
#include "unistd.h"
#include "sys/stat.h"
#include "semaphore.h"
#include "fcntl.h"

int shared_memory_fd;
void *shm_ptr;

sem_t *mutex_semaphore;
sem_t *put_semaphore;
sem_t *get_semaphore;

void down(sem_t *);
void up(sem_t *);

void print_status();

int was_interrupted = 0;

void init_ipc() {

    int access_0660 = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

    // init shared memory
    if((shared_memory_fd = shm_open(IPC_SHM_NAME, O_RDWR | O_CREAT | O_EXCL, access_0660)) == -1) {

        if((shared_memory_fd = shm_open(IPC_SHM_NAME, O_RDWR, access_0660)) == -1) {
            perror("init_ipc(), shm_open");
            exit(-1);
        }

        if((shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0)) == MAP_FAILED) {
            perror("init_ipc(), shm mmap");
            exit(-1);
        }

    } else {

        if(ftruncate(shared_memory_fd, SHM_SIZE) == -1) {
            perror("SHM file truncate error");
            exit(-1);
        }

        if((shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0)) == MAP_FAILED) {
            perror("init_ipc(), shm mmap");
            exit(-1);
        }

        *((int*)(shm_ptr+TASK_COUNTER_OFFSET)) = 0;
        *((int*)(shm_ptr+START_INDEX_OFFSET)) = 0;
    }

    // init semaphores
    if((mutex_semaphore = sem_open(IPC_SEM_MUTEX_NAME, O_CREAT | O_EXCL, access_0660, 1)) == SEM_FAILED) {
        if((mutex_semaphore = sem_open(IPC_SEM_MUTEX_NAME, 0)) == SEM_FAILED) {
            perror("init_ipc(), mutex semaphore init");
            exit(-1);
        }
    }

    if((put_semaphore = sem_open(IPC_SEM_PUT_NAME, O_CREAT | O_EXCL , access_0660, MAX_TASK_CAPACITY)) == SEM_FAILED) {
        if((put_semaphore = sem_open(IPC_SEM_PUT_NAME, 0)) == SEM_FAILED) {
            perror("init_ipc(), put semaphore init");
            exit(-1);
        }
    }

    if((get_semaphore = sem_open(IPC_SEM_GET_NAME, O_CREAT | O_EXCL , access_0660, 0)) == SEM_FAILED) {
        if((get_semaphore = sem_open(IPC_SEM_GET_NAME, 0)) == SEM_FAILED) {
            perror("init_ipc(), get_semaphore_init");
            exit(-1);
        }
    }
}

void put_task(task *task_ptr) {

    down(put_semaphore);
    down(mutex_semaphore);
    {
        int num_tasks_in_buffer = *((int *) (shm_ptr + TASK_COUNTER_OFFSET));
        task_ptr->awaiting = num_tasks_in_buffer;
        ++num_tasks_in_buffer;
        *((int *) (shm_ptr + TASK_COUNTER_OFFSET)) = num_tasks_in_buffer;

        int start_index = *((int *) (shm_ptr + START_INDEX_OFFSET));
        int put_index = start_index + num_tasks_in_buffer;
        put_index %= MAX_TASK_CAPACITY;

        int *num_location = (int *) (shm_ptr + put_index * sizeof(int));
        *(num_location) = task_ptr->number;
    }
    up(mutex_semaphore);
    up(get_semaphore);

}

void get_task(task *task_ptr) {

    down(get_semaphore);
    down(mutex_semaphore);

    {
        int num_tasks_in_buffer = *((int *) (shm_ptr + TASK_COUNTER_OFFSET));
        task_ptr->awaiting = --num_tasks_in_buffer;
        *((int *) (shm_ptr + TASK_COUNTER_OFFSET)) = num_tasks_in_buffer;

        int start_index = *((int *) (shm_ptr + START_INDEX_OFFSET));
        int get_index = start_index;
        ++start_index;
        start_index %= MAX_TASK_CAPACITY;

        *((int*)(shm_ptr+START_INDEX_OFFSET)) = start_index;
        int *num_location = (int *) (shm_ptr + sizeof(int) * get_index);
        task_ptr -> number = *(num_location);
    }

    up(mutex_semaphore);
    up(put_semaphore);

}

void down(sem_t *sem_id) {

    if(sem_wait(sem_id) == -1) {
        perror("Semaphore down");
        exit(-1);
    }
}

void up(sem_t *sem_id) {

    if(sem_post(sem_id) == -1) {
        perror("Semaphore up");
        exit(-1);
    }
}

void print_status() {

    int count = *((int *)shm_ptr + TASK_COUNTER_OFFSET);
    int start_index = *((int *) (shm_ptr + START_INDEX_OFFSET));

    printf("Count: %d, start_index: %d\n", count, start_index);

    for(int i = 0; i < MAX_TASK_CAPACITY; ++i) {
        int *num_ptr = shm_ptr + i * sizeof(int);
        printf("%d: %d\n", i+1, *num_ptr);
    }

}

void add_signal_handlers() {

    sigset_t full_mask;
    sigfillset(&full_mask);

    struct sigaction action;

    action.sa_handler = signal_handle;

    action.sa_mask = full_mask;

    if(sigaction(SIGINT, &action, NULL) < 0) {
        perror("Set signal handler");
        error("Sigaction error for SIGINT");
    }

}

void signal_handle(int unused) {
    was_interrupted = 1;
}

int should_continue() {
    return ! was_interrupted;
}

void cleanup() {

    init_ipc();

    if(-1 == sem_close(mutex_semaphore)) {
        perror("Removing mutex semaphore");
    }

    if(-1 == sem_close(get_semaphore)) {
        perror("Removing get semaphore");
    }

    if(-1 == sem_close(put_semaphore)) {
        perror("Removing put semaphore");
    }

//    if(-1 == shmctl(shared_memory_fd, IPC_RMID, NULL)) {
//        perror("Removing shared memory");
//    }

    printf("IPC artifacts removed\n");
}

