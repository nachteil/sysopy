#include <stdio.h>
#include <stdlib.h>
#define CHANGES_MADE 1
#define NO_CHANGES 2

void perform_bubble_sort(void* (*get_next_record)(void), void (*swap_records)(int, int), void (*set_file_start)(void), int rec_size_in_bytes, int num_records) {
    if(num_records <= 1)
    {
        return;
    }

    int last_unsorted_element_index;
    int were_changes_made = NO_CHANGES;

//    printf("sortuje sie\n");

    for(last_unsorted_element_index = num_records - 1; last_unsorted_element_index >= 1; --last_unsorted_element_index)
    {
        set_file_start();
//        printf("Loop running to: %d\n", last_unsorted_element_index);
        were_changes_made = NO_CHANGES;

        unsigned char * previous_record = (unsigned char *) get_next_record();
        unsigned char * current_record = NULL;
        int counter;

        for(counter = 1; counter <= last_unsorted_element_index; ++counter)
        {
            current_record = (unsigned char *) get_next_record();
            if(current_record[0] < previous_record[0])
            {
                swap_records(counter-1, counter);
                were_changes_made = CHANGES_MADE;
//                printf("\tSwap %u %u\n", current_record[0], previous_record[0]);
                free(current_record);

            } else {
                free(previous_record);
                previous_record = current_record;
            }
//            printf("\tCurrent: %u, previous: %u\n", *current_record, *previous_record);
        }

        if(were_changes_made == NO_CHANGES)
        {
            //break;
        }
    }

}