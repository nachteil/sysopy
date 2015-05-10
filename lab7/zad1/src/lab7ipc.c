#include "lab7ipc.h"
#include "stdio.h"
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include "sys/sem.h"
#include "stdlib.h"
#include "stdio.h"
#include "signal.h"
#include "common.h"

int shared_memory_id;
void *shm_ptr;

int mutex_semaphore_id;
int put_semaphore_id;
int get_semaphore_id;

void down(int);
void up(int);

void print_status();

int was_interrupted = 0;

void init_ipc() {

    // init shared memory
    key_t shm_key = ftok(IPC_BASE_DIR, IPC_SHM_ID);
    if((shared_memory_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0660)) == -1) {


        if((shared_memory_id = shmget(shm_key, SHM_SIZE, 0)) == -1) {
            perror("init_ipc(), shm get");
            exit(-1);
        }

        if((shm_ptr = shmat(shared_memory_id, 0, 0)) == (void *) -1) {
            perror("init_ipc(), shm attach");
            exit(-1);
        }

    } else {

        if((shm_ptr = shmat(shared_memory_id, 0, 0)) == (void *) -1) {
            perror("init_ipc(), shm attach");
            exit(-1);
        }

        *((int*)(shm_ptr+TASK_COUNTER_OFFSET)) = 0;
        *((int*)(shm_ptr+START_INDEX_OFFSET)) = 0;
    }


    // init semaphores
    key_t mutex_key = ftok(IPC_BASE_DIR, IPC_SEM_MUTEX_ID);
    key_t sem_get_key = ftok(IPC_BASE_DIR, IPC_SEM_GET_ID);
    key_t sem_put_key = ftok(IPC_BASE_DIR, IPC_SEM_PUT_ID);

    union semun arg;

    if((mutex_semaphore_id = semget(mutex_key, 1, IPC_CREAT | IPC_EXCL | 0660)) == -1) {
        if((mutex_semaphore_id = semget(mutex_key, 1, 0)) == -1) {
            perror("init_ipc(), mutex semaphore init");
            exit(-1);
        }
    } else {
        // set mutex semaphore initial value to 1
        arg.val = 1;
        semctl(mutex_semaphore_id, 0, SETVAL, arg);
    }

    if((put_semaphore_id = semget(sem_put_key, 1, IPC_CREAT | IPC_EXCL | 0660)) == -1) {
        if((put_semaphore_id = semget(sem_put_key, 1, IPC_CREAT)) == -1) {
            perror("init_ipc(), put semaphore init");
            exit(-1);
        }
    } else {
        // set put semaphore initial value to MAX_TASK_CAPACITY
        arg.val = MAX_TASK_CAPACITY;
        semctl(put_semaphore_id, 0, SETVAL, arg);
    }

    if((get_semaphore_id = semget(sem_get_key, 1, IPC_CREAT | IPC_EXCL | 0660)) == -1) {
        if((get_semaphore_id = semget(sem_get_key, 1, IPC_CREAT)) == -1) {
            perror("init_ipc(), get_semaphore_init");
            exit(-1);
        }
    } else {
        // set get semaphore initial value to 0
        arg.val = 0;
        semctl(get_semaphore_id, 0, SETVAL, arg);
    }
}

void put_task(task *task_ptr) {

    down(put_semaphore_id);
    down(mutex_semaphore_id);
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
    up(mutex_semaphore_id);
    up(get_semaphore_id);

}

void get_task(task *task_ptr) {

    down(get_semaphore_id);
    down(mutex_semaphore_id);

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

    up(mutex_semaphore_id);
    up(put_semaphore_id);

}

void down(int sem_id) {

    struct sembuf decr;

    decr.sem_num = 0;
    decr.sem_op = -1;
    decr.sem_flg = 0;

    if(semop(sem_id, &decr, 1) == -1) {
        perror("Semaphore down");
        exit(-1);
    }
}

void up(int sem_id) {

    struct sembuf incr;

    incr.sem_num = 0;
    incr.sem_op  = 1;
    incr.sem_flg = 0;

    if(semop(sem_id, &incr, 1) == -1) {
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

    if(-1 == semctl(mutex_semaphore_id, 0, IPC_RMID, NULL)) {
        perror("Removing mutex semaphore");
    }

    if(-1 == semctl(get_semaphore_id, 0, IPC_RMID, NULL)) {
        perror("Removing get semaphore");
    }

    if(-1 == semctl(put_semaphore_id, 0, IPC_RMID, NULL)) {
        perror("Removing put semaphore");
    }

    if(-1 == shmctl(shared_memory_id, IPC_RMID, NULL)) {
        perror("Removing shared memory");
    }

    printf("IPC artifacts removed\n");
}

