#define _GNU_SOURCE
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "main_1.h"
#include <sched.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/times.h>

#define FORK 1
#define VFORK 2
#define FORK_CLONE 3
#define VFORK_CLONE 4

#define CHILD_PROC_ID 0

static int counter;

static int  *global_variable;
static long *total_child_processes_time;

int main(int argc, char * argv[]) {

    if(argc != 3)
    {
        printf("Require two integer arguments: number of executions and process-creating function type:\n");
        printf("\t 1 - fork\n\t2 - vfork\n\t3 - clone equivalent of fork\n\t4 - clone equivalent of vfork");
        exit(1);
    }

    int N = atoi(argv[1]);
    int f_type = atoi(argv[2]);

    global_variable = mmap(NULL, sizeof *global_variable, PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *global_variable = 0;
    total_child_processes_time = mmap(NULL, sizeof *total_child_processes_time, PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *total_child_processes_time = 0;

    struct tms struct_times;
    long start_time = times(NULL);

    counter = 0;

    switch(f_type) {
        case FORK:
            do_fork(N);
            break;
        case VFORK:
            do_vfork(N);
            break;
        case FORK_CLONE:
            do_fork_clone(N);
            break;
        case VFORK_CLONE:
            do_vfork_clone(N);
            break;
        default:
            printf("Invalid function type, exit now\n");
            exit(1);
    }

    long end_time = times(&struct_times);
    double real_parent = end_time - start_time;
    double real_children = *total_child_processes_time;
    double user_parent = struct_times.tms_utime;
    double user_children = struct_times.tms_cutime;
    double sys_parent = struct_times.tms_stime;
    double sys_children = struct_times.tms_cstime;

    double ticks_per_second = sysconf(_SC_CLK_TCK);

    sys_parent /= ticks_per_second;
    sys_children /= ticks_per_second;
    user_parent /= ticks_per_second;
    user_children /= ticks_per_second;
    real_children /= ticks_per_second;
    real_parent /= ticks_per_second;


    // n,real_parent,real_children,sys_parent,sys_children,user_parent,user_children,counter,global
    printf("%d,%lf,%lf,%lf,%lf,%lf,%lf,%d,%d\n",
           N, real_parent, real_children, sys_parent, sys_children, user_parent, user_children, counter, *global_variable
    );

    exit(0);
}

int function(void *forking_start) {

    (*global_variable) = (*global_variable) + 1;
    ++counter;
    long function_end_time = times(NULL);
    (*total_child_processes_time) = (*total_child_processes_time) + (function_end_time - *((long*)forking_start));
    _exit(0);
}

void do_fork(int n) {

    for(int i = 0; i < n; ++i) {

        int fork_id;

        if ((fork_id = fork()) < 0) {
            printf("Forking error\n");
            exit(1);
        }
        long fork_start_time = times(NULL);

        // child process code
        if (fork_id == CHILD_PROC_ID) {
            ++counter;
            long child_end_time = times(NULL);
            (*total_child_processes_time) = (*total_child_processes_time) + (child_end_time - fork_start_time);
            (*global_variable) = (*global_variable) + 1;
            _exit(0);

            // parent process code
        } else {
            wait(NULL);
        }
    }
}

void do_vfork(int n) {

    for(int i = 0; i < n; ++i) {

        int fork_id;

        if ((fork_id = vfork()) < 0) {
            printf("Forking error\n");
            exit(1);
        }
        long fork_start_time = times(NULL);

        // child process code
        if (fork_id == CHILD_PROC_ID) {
            (*global_variable) = (*global_variable) + 1;
            ++counter;
            long child_end_time = times(NULL);
            (*total_child_processes_time) = (*total_child_processes_time) + (child_end_time - fork_start_time);
            _exit(0);

            // parent process code
        } else {
            wait(NULL);
        }
    }
}

void do_fork_clone(int n) {

    int stack_size = 1024;
    void *child_stack = (void *) malloc(stack_size);
    // remember, stack fork from the end!
    child_stack = (void *)((long) child_stack + stack_size);

    for(int i = 0 ; i < n; ++i) {

        int fork_id;
        long fork_start_time = times(NULL);

        if ((fork_id = clone(&function, child_stack, SIGCHLD, &fork_start_time)) < 0) {
            printf("Forking error\n");
            exit(1);
        }
        wait(NULL);
    }
}

void do_vfork_clone(int n) {

    int stack_size = 1024;
    void *child_stack = (void *) malloc(stack_size);
    // remember, stack fork from the end!
    child_stack = (void *)((long) child_stack + stack_size);

    for(int i = 0 ; i < n; ++i) {

        int fork_id;
        long fork_start_time = times(NULL);

        if ((fork_id = clone(&function, child_stack, SIGCHLD | CLONE_VFORK | CLONE_VM, &fork_start_time)) < 0) {
            printf("Forking error\n");
            exit(1);
        }
        wait(NULL);
    }
}