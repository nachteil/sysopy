#include "consumer.h"
#include "sys/types.h"
#include "sys/ipc.h"
#include "unistd.h"
#include "common.h"
#include "stdlib.h"
#include "lab7ipc.h"
#include "stdio.h"

char *line_buffer = NULL;

int main(int argc, char * argv []) {

    if((line_buffer = (char*)malloc(1024)) == NULL) {
        mem_error();
    }

    init_ipc();
    add_signal_handlers();

    while(should_continue()) {

        task t;
        get_task(&t);

        int number = t.number;
        int awaiting = t.awaiting;

        if(prime(number)) {
            print_output(number, "pierwsza", awaiting);
        } else {
            print_output(number, "zlozona", awaiting);
        }
        usleep(CONSUMER_SLEEP_MS * 1000);
    }

    exit(0);
}

int prime(int n) {

    for(int i = 2; i * i <= n; ++i) {
        if(n % i == 0) {
            return 0;
        }
    }
    return 1;
}

void print_output(int n, char *c, int awaiting) {

    char *timestamp = get_timestamp_with_millis();
    pid_t pid = getpid();

    printf("(%d %s) Sprawdzilem liczbe %d - %s. Pozostalo zasad oczekujacych: %d\n", (int) pid, timestamp, n, c, awaiting);

}