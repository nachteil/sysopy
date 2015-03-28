#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "sys/times.h"

static long int ticks_per_sec = CLOCKS_PER_SEC;

struct tms *run_time;
struct tms previous;

double prev_usr_milis = 0.0d;
double prev_sys_milis = 0.0d;
double prev_real_milis = 0.0d;

int main(int argc, char * argv[]) {

    printf("Program started. Clocks per second: %ld\n", ticks_per_sec);

    run_time = (struct tms *) malloc(sizeof(struct tms));
    verify_not_null(times);

    print_time();

    list_t * list = create_sample_list();

    printf("List created:\n");

    print_list(list);

    print_time();

    printf("-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\n\n");

    int i;
    for(i = 0; i < 100000; ++i) {
        list_sort(list);
    }

    printf("List sorted 100000 times\n");
    print_time();

    for(i = 0; i < 10000000; ++i) {
        if(atoi("AGH") == 5)
        {
            if(1 < -1) printf("OK");
        }
    }

    print_list(list);

    list_node_t * node = find_elem_by_full_name(list, "Kora", "Rako");
    printf("Node found: %s %s\nAddress: %s %s St,. %s %s\n", node -> name, node -> surname, node -> address -> building_num,
    node -> address -> street, node -> address -> city, node -> address -> zip_code);

    printf("Program almost ended\n");
    print_time();

    return 0;
}

void print_time()
{
    previous = *run_time;
    times(run_time);

    double total_usr = (double) run_time -> tms_utime;
    double total_sys = (double) run_time -> tms_stime;
    double total_real =  total_sys + total_usr;


    double total_usr_milis = (total_usr / ticks_per_sec) * 10000.0d;
    double total_sys_milis = (total_sys / ticks_per_sec) * 10000.0d;
    double total_real_milis = (total_real / ticks_per_sec) * 10000.0d;

    printf("\n* * * * * * * * * * * * * * * * * * T I ME\tM E A S U R E M E N T* * * * * * * * * * * * * * * * * *");
    printf("\nTotal time:\n\treal: %5.3lf\n\tusr: %5.3lf ms\n\tsys: %5.3lf ms\n", total_real_milis, total_usr_milis, total_sys_milis);
    printf("\nSince last measurement:\n\tdelta real: %5.3lf\n\tdelta usr: %5.3lf ms\n\tdelta sys: %5.3f ms\n", total_real_milis - prev_real_milis, total_usr_milis - prev_usr_milis, total_sys_milis - prev_usr_milis);

    prev_usr_milis = total_usr_milis;
    prev_sys_milis = total_sys_milis;
}
