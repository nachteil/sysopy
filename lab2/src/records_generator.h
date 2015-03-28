//
// Created by yorg on 17.03.15.
//

#include <stdio.h>

#ifndef _SYSOPY_MAIN_H_
    #define _SYSOPY_MAIN_H_
#endif //_SYSOPY_MAIN_H_

void usage();
void write_records(FILE *file_handler, int rec_size, int num_rec);
void print_error_message(const char * msg);