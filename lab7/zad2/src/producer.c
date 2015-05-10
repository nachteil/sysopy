#include "producer.h"
#include "stdlib.h"
#include "time.h"
#include "sys/types.h"
#include "unistd.h"
#include "common.h"
#include "lab7ipc.h"
#include "stdio.h"

int main(int argc, char * argv[]) {

    init_ipc();

    int seed = (int ) (((long) time((void *)0) * (long) clock()) % 1000000);
    srand(seed);

    while(should_continue()) {

        task t;
        t.number = rand() % NUM_MAX + 1;

        put_task(&t);
        print_note(t.number, t.awaiting);

        usleep(PRODUCER_SLEEP_MS * 1000);
    }

    exit(0);
}

void print_note(int num, int awaiting) {

    pid_t pid = getpid();
    char *timestamp = get_timestamp_with_millis();
    printf("(%d %s) Dodalem liczbe: %d. Liczba zadan oczekujacych: %d\n", (int) pid, timestamp, num, awaiting);
}