#define _GNU_SOURCE

#include "parent.h"
#include "stdlib.h"
#include "stdio.h"
#include "common.h"
#include "signal.h"

int counter = 0;
int flag = 0;
int double_flag = 0;
int confirmation = 0;

pid_t child_pid;

int main(int argc, char * argv[]) {

    if (argc != 2) {
        printf("Usage: singals N\n\tN - number of signals to send between processess\n");
        exit(1);
    }

    sigset_t zero_mask;
    sigset_t usr_mask;

    if(sigemptyset(&zero_mask) < 0) {
        error("Sigemptyset error");
    }

    if(sigemptyset(&usr_mask) < 0) {
        error("Sigemptyset error");
    }
    if(sigaddset(&usr_mask, SIGUSR1) < 0) {
        error("Sigaddset error");
    }
    if(sigaddset(&usr_mask, SIGUSR2) < 0) {
        error("Sigaddset error");
    }

    struct sigaction new_act_usr1;
    struct sigaction new_act_usr2;

    new_act_usr1.sa_handler = sig_handle_sender;
    new_act_usr2.sa_handler = sig_handle_sender;

    new_act_usr1.sa_mask = usr_mask;
    new_act_usr2.sa_mask = usr_mask;

    if(sigaction(SIGUSR1, &new_act_usr1, NULL) < 0) {
        error("Sigaction error for SIGUSR1");
    }
    if(sigaction(SIGUSR2, &new_act_usr2, NULL) < 0) {
        error("Sigaction error for SIGUSR2");
    }

    int num_signals;
    num_signals = atoi(argv[1]);

    pid_t pid;

    if((pid = fork()) < 0) {
        error("Fork error");

    // child process
    } else if (pid == 0) {
        printf("Child - after fork, calling exec\n");
        execl("./bin/child", "child", NULL);
        error("Should not get here after exec!");
    }
    // parent process - just carry on

    child_pid = pid;

    // wait for child to confirm ready
    flag = 0;
    while(!flag) {
        pause();
    }
    flag = 0;

    for(int i = 0; i < num_signals; ++i) {
        if(kill(pid, SIGUSR1) < 0) {
            error("Kill SIGUSR1 errir");
        }
        while(!confirmation) {
            pause();
        }
        confirmation = 0;
    }

    // switch handlers to receivers and go to receive mode
    new_act_usr1.sa_handler = sig_handle_receiver;
    new_act_usr2.sa_handler = sig_handle_receiver;

    if(sigaction(SIGUSR1, &new_act_usr1, NULL) < 0) {
        error("Sigaction error for SIGUSR1");
    }
    if(sigaction(SIGUSR2, &new_act_usr2, NULL) < 0) {
        error("Sigaction error for SIGUSR1");
    }
    if(kill(pid, SIGUSR2) < 0) {
        error("Kill SIGUSR2 error");
    }

    while(!flag) {
        pause();
    }

    printf("Parent - received back %d out of %d signals\n", counter, num_signals);
}

void sig_handle_sender(int signo) {

    if(signo == SIGUSR1) {
        confirmation = 1;
    } else if (signo == SIGUSR2){
        flag = 1;
    } else {
        error("Unexpected signal");
    }
}

void sig_handle_receiver(int signo) {

    if(signo == SIGUSR1) {
        ++counter;
        kill(child_pid, SIGUSR1);
    } else if (signo == SIGUSR2) {
        flag = 1;
    } else {
        error("Unexpected signal");
    }
}