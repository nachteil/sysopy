#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "main_1.h"
#include <sched.h>

#define IM_A_CHILD 0

static int counter = 0;

// bardziej zaawansowana wersja - nie dawać kodu do wykonywania w procesie potomnym,
// tylko zwiększyć rozmiar pamięci, która jest kopiowana przy tworzeniu procesu

int main(int argc, char * argv[]) {

    if(argc < 3)
    {
        printf("Require two integer arguments: number of executions and process-creating function type:\n");
        printf("\t 1 - fork\n\t2 - vfork\n\t3 - clone equivalent of fork\n\t4 - clone equivalent of vfork");
        exit(1);
    }

    int N = atoi(argv[1]);
    int f_type = atoi(argv[2]);

    int (*fun_ptr)(void) = NULL;

    switch (f_type)
    {
        case 1:
            fun_ptr = &f_fork;
            break;
        case 2:
            fun_ptr = &f_vfork;
            break;
        case 3:
            fun_ptr = &clone_fork;
            break;
        case 4:
            fun_ptr = &clone_vfork;
            break;
        default:
            printf("Unrecognized function type. Exiting now.");
            exit(1);
    }

    int i;
    for(i = 0; i < N; ++i)
    {
        int fork_id = fun_ptr();
        if(fork_id == IM_A_CHILD) // executed in child process
        {
            ++counter;
            _exit(0);
        } else // executed in parent process
        {
            waitpid(fork_id, NULL, 0);
        }
    }

    printf("Counter after %d iterations: %d\n", N, counter);

    return 0;
}

int f_fork()
{
    return fork();
}

int f_vfork()
{
    return vfork();
}

int increment_counter()
{
    printf("PID: %d, increment counter\n", getpid());
    ++counter;
    _exit(0);
}

int clone_fork()
{
    void *child_stack = malloc(1024);
    pid_t child = clone( increment_counter, child_stack,
            /* int flags                */     SIGCHLD,
            /* argument to child_f      */     NULL,
            /* pid_t *pid               */     NULL,
            /* struct usr_desc * tls    */     NULL,
            /* pid_t *ctid              */     NULL );

    return child;
}

int clone_vfork()
{
    printf("Clone vfork\n");
    void *child_stack = malloc(1024);
    pid_t child = clone( increment_counter, child_stack,
            /* int flags                */     SIGCHLD | CLONE_VM,
            /* argument to child_f      */     NULL,
            /* pid_t *pid               */     NULL,
            /* struct usr_desc * tls    */     NULL,
            /* pid_t *ctid              */     NULL );

    return child;
}