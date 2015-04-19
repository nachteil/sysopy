#define _GNU_SOURCE

#include "parent.h"
#include "stdlib.h"
#include "stdio.h"
#include "common.h"
#include "signal.h"

int counter = 0;
int flag = 0;
int double_flag = 0;

pid_t child_pid;

int main(int argc, char * argv[]) {

    if (argc != 2) {
        printf("Usage: singals N\n\tN - number of signals to send between processess\n");
        exit(1);
    }

    sigset_t zero_mask;
    sigemptyset(&zero_mask);

    if(signal(SIGUSR1, sig_handle_sender) == SIG_ERR) {
        error("Can't catch SIGUSR1");
    }
    if(signal(SIGUSR2, sig_handle_sender) == SIG_ERR) {
        error("Can't catch SIGUSR2");
    }

    int num_signals;
    num_signals = atoi(argv[1]);

    pid_t pid;

    if((pid = fork()) < 0) {
        error("Fork error");

    // child process
    } else if (pid == 0) {
        printf("Child - after fork, calling exec\n");
        fflush(stdout);
        execl("./bin/child", "child", NULL);
        error("Should not get here after exec!");
    }
    // parent process - just carry on

    printf("Parent - after fork\n");
    fflush(stdout);
    child_pid = pid;

    // wait for child to confirm ready
    flag = 0;
    printf("Parent - wait for child\n");
    fflush(stdout);
    while(!flag) {
        sigsuspend(&zero_mask);
    }
    flag = 0;

    printf("Parent - child confirmed ready\n");
    fflush(stdout);

    for(int i = 0; i < num_signals; ++i) {
        kill(pid, SIGUSR1);
        sigsuspend(&zero_mask);
    }

    // switch handlers to receivers and go to receive mode
    signal(SIGUSR1, sig_handle_receiver);
    signal(SIGUSR2, sig_handle_receiver);
    kill(pid, SIGUSR2);

    while(!flag) {
        sigsuspend(&zero_mask);
    }

    printf("Received back %d out of %d signals\n", counter, num_signals);
}

void sig_handle_sender(int signo) {

    signal(SIGUSR1, sig_handle_sender);
    signal(SIGUSR2, sig_handle_sender);

    if(signo == SIGUSR1) {
        // just ignore
    } else if (signo == SIGUSR2){
        flag = 1;
        printf("Parent - signal2\n");
    } else {
        error("Unexpected signal");
    }
}

void sig_handle_receiver(int signo) {

    signal(SIGUSR1, sig_handle_receiver);
    signal(SIGUSR2, sig_handle_receiver);

    if(signo == SIGUSR1) {
        printf("Parent - got signal\n");
        fflush(stdout);
        ++counter;
        kill(child_pid, SIGUSR1);
        printf("Parent - sent confirmation\n");
        fflush(stdout);
    } else if (signo == SIGUSR2) {
        flag = 1;
    } else {
        error("Unexpected signal");
    }
}