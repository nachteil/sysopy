#ifndef SYSOPY_LAB7IPC_H
#define SYSOPY_LAB7IPC_H
#define _GNU_SOURCE

#define IPC_BASE_DIR "."
#define IPC_SHM_ID 1
#define IPC_SEM_GET_ID 2
#define IPC_SEM_PUT_ID 3
#define IPC_SEM_MUTEX_ID 4

#define MAX_TASK_CAPACITY 25
#define SHM_SIZE (25 * sizeof(int) + 2*sizeof(int))
#define TASK_COUNTER_OFFSET (25 * sizeof(int))
#define START_INDEX_OFFSET (25*sizeof(int) + sizeof(int))

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;
};

typedef struct task {

    int number;
    int awaiting;

} task;

void init_ipc();
void put_task(task *);
void get_task(task *);

void add_signal_handlers();
void signal_handle(int);
int should_continue();

void cleanup();

#endif //SYSOPY_LAB7IPC_H
