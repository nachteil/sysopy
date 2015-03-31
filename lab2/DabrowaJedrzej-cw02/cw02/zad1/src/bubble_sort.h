#ifndef _SYSOPY_BUBBLE_SORT_H_
#define _SYSOPY_BUBBLE_SORT_H_

void perform_bubble_sort(void* (*get_next_record)(void), void (*swap_records)(int, int), void (*set_file_start)(void), int rec_size_in_bytes, int num_records);


#endif //_SYSOPY_BUBBLE_SORT_H_
