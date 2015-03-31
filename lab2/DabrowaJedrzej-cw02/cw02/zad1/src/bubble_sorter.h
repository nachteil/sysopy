#ifndef _SYSOPY_BUBBLE_SORTER_H_
#define _SYSOPY_BUBBLE_SORTER_H_

void swap_records_std_io(int a, int b);
void swap_records_sys_io(int a, int b);

void * get_next_record_sys_io();
void * get_next_record_std_io();

void set_file_start_std_io();
void set_file_start_sys_io();

#endif //_SYSOPY_BUBBLE_SORTER_H_

