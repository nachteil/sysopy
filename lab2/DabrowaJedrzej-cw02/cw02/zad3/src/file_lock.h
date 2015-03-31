#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#ifndef _SYSOPY_FILE_LOCK_H_
#define _SYSOPY_FILE_LOCK_H_

typedef struct command {
    char *cmd;
    int num;
    int cmd_index;
} command;


command *parse(char *);
void try_lock(command *);
void print_lock_info(struct flock *);
void print_locks(void);

#endif //_SYSOPY_FILE_LOCK_H_
