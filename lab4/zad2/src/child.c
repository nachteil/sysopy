#include "child.h"
#include "common.h"
#include "sys/types.h"
#include "unistd.h"
#include "signal.h"

static int counter;
static int proceed;

int main(int argc, char *argv[]) {

    counter = 0;
    proceed = 0;

    printf("Child - started main after exec...\n");
    fflush(stdout);

    sigset_t zero_mask;
    sigemptyset(&zero_mask);

    if(signal(SIGUSR1, sig_handle) == SIG_ERR) {
        error("Cannot catch SIGUSR1");
    }
    if(signal(SIGUSR2, sig_handle) == SIG_ERR) {
        error("Cannot catch SIGUSR2");
    }

    // everything's ready - we can start. Inform parent.
    printf("Inform parent I'm ready\n");
    fflush(stdout);
    kill(getppid(), SIGUSR2);

    while(!proceed) {
        sigsuspend(&zero_mask);
    }

    printf("Child: received %d signals in total\n", counter);
    pid_t ppid = getppid();
    if(signal(SIGUSR1, sig_handle_sender) == SIG_ERR) {
        error("Cannot catch SIGUSR1");
    }

    // resend signals
    for(int i = 0; i < counter; ++i) {
        printf("Child - send signal %d\n", i+1);
        fflush(stdout);
        kill(ppid, SIGUSR1);
        sigsuspend(&zero_mask);
        printf("Child - got confirmation\n");
        fflush(stdout);
    }
    kill(ppid, SIGUSR2);

    exit(0);
}

void sig_handle(int signo) {

    signal(SIGUSR1, sig_handle);

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

    signal(SIGUSR1, sig_handle_sender);

    if(signo == SIGUSR1) {
        // ignore, it's just confirmation
    } else {
        error("Wrong signal");
    }
}