#ifndef _SYSOPY_SINGALS_H_
#define _SYSOPY_SINGALS_H_

void execute_child_process();
void execute_parent_process(int, int);

void child_signal_handler(int signo)
void parent_signal_handler(int signo)

#endif //_SYSOPY_SINGALS_H_
