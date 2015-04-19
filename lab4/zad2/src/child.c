#include "child.h"
#include "common.h"
#include "sys/types.h"
#include "unistd.h"
#include "signal.h"

static int counter;
static int proceed;
static int confirmation = 0;

int main(int argc, char *argv[]) {

    counter = 0;
    proceed = 0;

    printf("Child - started main after exec...\n");
    fflush(stdout);

    sigset_t zero_mask;
    sigemptyset(&zero_mask);

    sigset_t usr_mask;
    sigemptyset(&usr_mask);
    sigaddset(&usr_mask, SIGUSR1);
    sigaddset(&usr_mask, SIGUSR2);

    struct sigaction new_act_usr1;
    struct sigaction new_act_usr2;

    new_act_usr1.sa_handler = sig_handle;
    new_act_usr2.sa_handler = sig_handle;

    new_act_usr1.sa_mask = usr_mask;
    new_act_usr2.sa_mask = usr_mask;

    if(sigaction(SIGUSR1, &new_act_usr1, NULL) < 0) {
        error("Sigaction error for SIGUSR1");
    }
    if(sigaction(SIGUSR2, &new_act_usr2, NULL) < 0) {
        error("Sigaction error for SIGUSR2");
    }

    // everything's ready - we can start. Inform parent.
    kill(getppid(), SIGUSR2);

    while(!proceed) {
        pause();
    }

    printf("Child: received %d signals in total\n", counter);
    pid_t ppid = getppid();
    if(signal(SIGUSR1, &sig_handle_sender) == SIG_ERR) {
        error("Cannot catch SIGUSR1");
    }

    // resend signals
    for(int i = 0; i < counter; ++i) {
        kill(ppid, SIGUSR1);
        while(!confirmation) {
            pause();
        }
        confirmation = 0;
    }
    kill(ppid, SIGUSR2);

    exit(0);
}

void sig_handle(int signo) {

    if(signo == SIGUSR1) {
        ++counter;
        kill(getppid(), SIGUSR1);
    } else if (signo == SIGUSR2) {
        proceed = 1;
    } else {
        error("Wrong signal");
    }
}

void sig_handle_sender(int signo) {

    if(signo == SIGUSR1) {
        confirmation = 1;
    } else {
        error("Wrong signal");
    }
}