#include "child.h"
#include "common.h"
#include "sys/types.h"
#include "unistd.h"
#include "signal.h"

#define SIGRTUSR1 SIGRTMIN+1
#define SIGRTUSR2 SIGRTMIN+2

static int counter;
static int proceed;

int main(int argc, char *argv[]) {

    if(SIGRTUSR1 > SIGRTMAX || SIGRTUSR2 > SIGRTMAX) {
        error("Defined real-time singal values exceed SIGRTMAX");
    }

    counter = 0;
    proceed = 0;

    sigset_t zero_mask;
    sigemptyset(&zero_mask);

    if(signal(SIGRTUSR1, sig_handle) == SIG_ERR) {
        error("Cannot catch SIGRTUSR1");
    }
    if(signal(SIGRTUSR2, sig_handle) == SIG_ERR) {
        error("Cannot catch SIGRTUSR2");
    }

    // everything's ready - we can start. Inform parent.
    kill(getppid(), SIGRTUSR2);

    while(!proceed) {
        sigsuspend(&zero_mask);
    }

    printf("Child: received %d signals in total\n", counter);
    pid_t ppid = getppid();
    for(int i = 0; i < counter; ++i) {
        kill(ppid, SIGRTUSR1);
    }
    kill(ppid, SIGRTUSR2);

    exit(0);
}

void sig_handle(int signo) {

    signal(SIGRTUSR1, sig_handle);

    if(signo == SIGRTUSR1) {
        ++counter;
    } else if (signo == SIGRTUSR2) {
        proceed = 1;
    } else {
        error("Wrong signal");
    }

}