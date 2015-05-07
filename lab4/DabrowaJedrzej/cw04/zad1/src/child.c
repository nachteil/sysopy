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

    sigset_t zero_mask;
    sigemptyset(&zero_mask);

    if(signal(SIGUSR1, sig_handle) == SIG_ERR) {
        error("Cannot catch SIGUSR1");
    }
    if(signal(SIGUSR2, sig_handle) == SIG_ERR) {
        error("Cannot catch SIGUSR2");
    }

    // everything's ready - we can start. Inform parent.
    if(kill(getppid(), SIGUSR2) < 0) {
        error("Kill SIGUSR2 error");
    }

    while(!proceed) {
        sigsuspend(&zero_mask);
    }

    printf("Child: received %d signals in total\n", counter);
    pid_t ppid = getppid();
    for(int i = 0; i < counter; ++i) {
        if(kill(ppid, SIGUSR1) < 0) {
            error("Kill SIGUSR1 error");
        }
        // uncomment, to verify, that signals are sent, but lost
//         sleep(1);
    }
    if(kill(ppid, SIGUSR2) < 0) {
        error("Kill SIGUSR2 error");
    }

    exit(0);
}

void sig_handle(int signo) {

    if(signal(SIGUSR1, sig_handle) < 0) {
        error("Signal SIGUSR1 error");
    }

    if(signo == SIGUSR1) {
        ++counter;
    } else if (signo == SIGUSR2) {
        proceed = 1;
    } else {
        error("Wrong signal");
    }

}