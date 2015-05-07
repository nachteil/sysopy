#define _GNU_SOURCE

#include "parent.h"
#include "stdlib.h"
#include "stdio.h"
#include "common.h"
#include "signal.h"

#define SIGRTUSR1 SIGRTMIN+1
#define SIGRTUSR2 SIGRTMIN+2

int counter = 0;
int flag = 0;

int main(int argc, char * argv[]) {

    if(SIGRTUSR1 > SIGRTMAX || SIGRTUSR2 > SIGRTMAX) {
        error("Defined real-time singal values exceed SIGRTMAX");
    }

    if (argc != 2) {
        printf("Usage: singals N\n\tN - number of signals to send between processess\n");
        exit(1);
    }

    sigset_t zero_mask;
    sigemptyset(&zero_mask);

    if(signal(SIGRTUSR1, sig_handle) == SIG_ERR) {
        error("Can't catch SIGRTUSR1");
    }
    if(signal(SIGRTUSR2, sig_handle) == SIG_ERR) {
        error("Can't catch SIGRTUSR2");
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
        kill(pid, SIGRTUSR1);
    }
    kill(pid, SIGRTUSR2);

    while(!flag) {
        sigsuspend(&zero_mask);
    }

    printf("Received back %d out of %d signals\n", counter, num_signals);
}

void sig_handle(int signo) {

    signal(SIGRTUSR1, sig_handle);
    signal(SIGRTUSR2, sig_handle);

    if(signo == SIGRTUSR1) {
        ++counter;
    } else if (signo == SIGRTUSR2) {
        flag = 1;
    } else {
        error("Unexpected signal");
    }

}