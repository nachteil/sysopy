#include "singals.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>


// should use sigsuspend nad masks
// also: set block before fork invokation, to avoid terminating child by signal from parent process

#define CHILD_PROC_ID 0

int signal_counter = 0;
int should_end = 0;

sigset_t *mask;

int main(int argc, char * argv[]) {

    if(argc != 2) {
        printf("Usage: singals N\n\tN - number of signals to send between processess\n");
        exit(1);
    }

    int num_signals;
    num_signals = atoi(argv[1]);

    int fork_id;
    fork_id = fork();

    if(signal(SIGUSR1, child_signal_handler()) == SIG_ERR) {
        printf("Cannot catch SIGUSR1\n");
        exit(1);
    }

    if(signal(SIGUSR2, child_signal_handler()) == SIG_ERR) {
        printf("Cannot catch SIGUSR2\n");
        exit(1);
    }

    mask = malloc(sizeof(sigset_t));
    sigemptyset(mask);

    if(fork_id == CHILD_PROC_ID) { // execution in child process
        execute_child_process();
        return 0;
    } else { // execution in parent process
        execute_parent_process(num_signals, fork_id);
        return 0;
    }
}

void execute_child_process() {

    while(!should_end) {
        pause();
    }

}

void execute_parent_process(int num_signals, int child_pid) {

    int i = 0;
    for( ; i < num_signals; ++i) {
        kill(child_pid, SIGUSR1);
    }
    kill(child_pid, SIGUSR2);

    while(!should_end) {
        pause();
    }

}

void child_signal_handler(int signo) {

    //handle SIGUSR1 - count signals
    if(signo == SIGUSR1) {

        ++signal_counter;

    // handle SIGUSR2 - re-send signals
    } else if(signo == SIGUSR2){

        int ppid = getppid();
        int i = 0;
        for( ; i < signal_counter; ++i) {
            kill(ppid, SIGUSR1);
        }
        kill(ppid, SIGUSR2);
        should_end = 1;

    // handle other signals
    } else {
        printf("Unrecognized signal: %d, aborting\n", signo);
        exit(1);
    }
}

void parent_signal_handler(int signo) {

    //handle SIGUSR1 - count signals
    if(signo == SIGUSR1) {

        ++signal_counter;

        // handle SIGUSR2 - re-send signals
    } else if(signo == SIGUSR2){

        printf("Parent: received %d signals (expected %d). Ending program.\n");
        should_end = 1;

        // handle other signals
    } else {
        printf("Parent: unrecognized signal: %d, aborting\n", signo);
        exit(1);
    }
}