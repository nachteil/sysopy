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
    if(sigemptyset(&zero_mask) < 0) {
        error("Sigemptyset error");
    }

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
        // don't check it, it must return -1 (and errno indicates interruption)
        sigsuspend(&zero_mask);
    }
    flag = 0;

    for(int i = 0; i < num_signals; ++i) {
        if(kill(pid, SIGUSR1) < 0) {
            error("Kill SIGUSR1 error");
        }
        // uncomment, to verify, that signals are sent, but lost
//        sleep(1);
    }
    if(kill(pid, SIGUSR2) < 0) {
        error("Kill SIGUSR2 error");
    }

    while(!flag) {
        sigsuspend(&zero_mask);
    }

    printf("Received back %d out of %d signals\n", counter, num_signals);
}

void sig_handle(int signo) {

    if(signal(SIGUSR1, sig_handle) < 0 || signal(SIGUSR2, sig_handle) < 0) {
        error("signal() function error");
    }

    if(signo == SIGUSR1) {
        ++counter;
    } else if (signo == SIGUSR2) {
        flag = 1;
    } else {
        error("Unexpected signal");
    }

}