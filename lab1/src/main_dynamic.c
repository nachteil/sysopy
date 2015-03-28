#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <list_structs.h>
#include "sys/times.h"
#include <dlfcn.h>

static long int ticks_per_sec = CLOCKS_PER_SEC;

struct tms *run_time;
struct tms previous;

double prev_usr_milis;
double prev_sys_milis;

int main(int argc, char * argv[]) {

    prev_sys_milis = prev_usr_milis = 0.0d;

    void * list_lib_handle = dlopen("/dupa.so.1.0", RTLD_NOW);

    if(list_lib_handle == NULL) {
        printf("\nERROR: Couldn't load library. Error message:\n%s\n", dlerror());
    }

    void (*verify_not_null)(void *)               = dlsym(list_lib_handle, "verify_not_null");
    list_t* (*create_sample_list)(void)         = dlsym(list_lib_handle, "create_sample_list");
    void (*add_elem)(list_t *, list_node_t*)    = dlsym(list_lib_handle, "add_elem");
    void (*delete_list)(list_t *)               = dlsym(list_lib_handle, "delete_list");
    void (*list_sort)(list_t *)                 = dlsym(list_lib_handle, "list_sort");
    list_node_t* (*find_elem_by_full_name)(list_t *, char *, char *) = dlsym(list_lib_handle, "find_elem_by_full_name");

    printf("Program started. Clocks per second: %ld\n\n", ticks_per_sec);

    run_time = (struct tms *) malloc(sizeof(struct tms));
    verify_not_null(times);

    print_time();

    list_t * list = create_sample_list();

    int i;
    for(i = 0; i < 1000; ++i) {
        int k;
        list_t * not_needed_list = create_sample_list();
        for(k = 0; k < 100; ++k) {
            list_t * tmp = create_sample_list();
            list_node_t * node = tmp -> head;
            while(node != NULL) {
                add_elem(not_needed_list, node);
                node = node -> next;
            }
        }
        delete_list(not_needed_list);
    }

    printf("Lists created:\n\n");


    print_time();

    for(i = 0; i < 10000000; ++i) {
        list_sort(list);
    }

    printf("List sorted 10 000 000 times\n\n");
    print_time();

    for(i = 0; i < 100000000; ++i) {
        if(atoi("AGH") == 5)
        {
            if(1 < -1) printf("OK");
        }
    }


    find_elem_by_full_name(list, "Kora", "Rako");

    printf("Program almost ended\n\n");
    print_time();

    return 0;
}

void print_time()
{
    previous = *run_time;
    long total = times(run_time);

    double total_usr = (double) run_time -> tms_utime;
    double total_sys = (double) run_time -> tms_stime;
    double total_real = total_sys + total_usr;


    double total_usr_milis = (total_usr / ticks_per_sec) * 10000.0d;
    double total_sys_milis = (total_sys / ticks_per_sec) * 10000.0d;
    double total_real_milis = (total_real / ticks_per_sec) * 10000.0d;

    printf("\tTotal time:  \t\t%4.3lf (+%4.3lf) s\n", total_usr_milis + total_sys_milis, total_usr_milis + total_sys_milis - prev_usr_milis - prev_sys_milis);
    printf("\tUser time:   \t\t%4.3lf (+%4.3lf) s\n", total_usr_milis, total_usr_milis - prev_usr_milis);
    printf("\tSystem time: \t\t%4.3lf (+%4.3lf) s\n\n", total_sys_milis, total_sys_milis - prev_sys_milis);

    prev_usr_milis = total_usr_milis;
    prev_sys_milis = total_sys_milis;
}
