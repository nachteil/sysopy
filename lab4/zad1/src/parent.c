#define _GNU_SOURCE

#include "parent.h"
#include "stdlib.h"
#include "stdio.h"
#include "common.h"
#include "signal.h"

int counter = 0;
int flag = 0;

int main(int argc, char * argv[]) {

    if (argc != 2) {
        printf("Usage: singals N\n\tN - number of signals to send between processess\n");
        exit(1);
    }

    sigset_t zero_mask;
    sigemptyset(&zero_mask);

    if(signal(SIGUSR1, sig_handle) == SIG_ERR) {
        error("Can't catch SIGUSR1");
    }
    if(signal(SIGUSR2, sig_handle) == SIG_ERR) {
        error("Can't catch SIGUSR2");
    }

    int num_signals;
    num_signals = atoi(argv[1]);

    pid_t pid;

    if((pid = fork()) < 0) {
        error("Fork error");

    // child process
    } else if (pid == 0) {
        execl("./bin/child", "child", NULL);
        error("Should not get here after exec!");
    }
    // parent process - just carry on

    // wait for child to confirm ready
    flag = 0;
    while(!flag) {
        sigsuspend(&zero_mask);
    }
    flag = 0;

    for(int i = 0; i < num_signals; ++i) {
        kill(pid, SIGUSR1);
        // uncomment, to verify, that signals are sent, but lost
        sleep(1);
    }
    kill(pid, SIGUSR2);

    while(!flag) {
        sigsuspend(&zero_mask);
    }

    printf("Received back %d out of %d signals\n", counter, num_signals);
}

void sig_handle(int signo) {

    signal(SIGUSR1, sig_handle);
    signal(SIGUSR2, sig_handle);

    if(signo == SIGUSR1) {
        ++counter;
    } else if (signo == SIGUSR2) {
        flag = 1;
    } else {
        error("Unexpected signal");
    }

}