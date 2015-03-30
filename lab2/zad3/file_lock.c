#include "file_lock.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libexplain/fcntl.h>

#define STRDLCK "set_read_lock"
#define STWRLCK "set_write_lock"
#define PRTLCKS "print_locks"
#define FRELCK  "free_lock"
#define RDCAHR  "read_character"
#define WRTCHR  "write_character"

#define STRDLCK_IND 1
#define STWRLCK_IND 2
#define PRTLCKS_IND 3
#define FRELCK_IND  4
#define RDCAHR_IND  5
#define WRTCHR_IND  6

int sys_file_descriptor;
char * file_path;

int main(int argc, char * argv[]) {

    if(argc != 2) {
        printf("You must specify a root path.\n");
        exit(1);
    }

    if(access(argv[1], F_OK) != 0) {
        printf("Cannot open specified path.\n");
        exit(1);
    }

    int arg_len = strlen(argv[1]);
    if((file_path = malloc(arg_len+1)) == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }

    strcpy(file_path, argv[1]);

    int sys_file_descr = -1;
    if((sys_file_descriptor = open(argv[1], O_RDWR)) == -1) {
        printf("The file %s could not have been opened\n", argv[1]);
        exit(1);
    } else {
        printf("File %s has been successfully opened\n", argv[1]);
    }

    printf("Usage: enter one of the following commands (n is index of the byte (character)\n");
    printf("\t%s n\n", STRDLCK);
    printf("\t%s n\n", STWRLCK);
    printf("\t%s n\n", FRELCK);
    printf("\t%s n\n", RDCAHR);
    printf("\t%s n\n", WRTCHR);
    printf("\t%s\n", PRTLCKS);
    printf("\t Enter 'exit' (no quotes) to exit\n");

    char input[256];

    while(1) {
        printf("> ");
        gets(input);
        command *cmd = parse(input);
        if(cmd == NULL) continue;
        printf("DEBUG: %s %d (%d)\n", cmd ->cmd, cmd -> num, cmd -> cmd_index);
        try_lock(cmd);
    }
}

command *parse(char * input) {

    if(!strcmp("exit", input)) {
        printf("Ending program\n");
        exit(0);
    }

    int len = strlen(input);
    int c = len-1;
    while((c >= 0) && (input[c] == ' ' || input[c] == '\t')) {
        input[c] = 0;
        --c;
    }

    len = strlen(input);
    if(len < 11) {
        printf("dupa\n");
        return NULL;
    }

    int num_spaces = 0;
    int space_index = 0;
    c = 0;
    while(c < len) {
        if(input[c++] == ' ') {
            ++num_spaces;
            space_index = c - 1;
        }
    }

    char * cmd;
    if((cmd = malloc(space_index + 1)) == NULL) {
        printf("Memory allocation error during parsing\n");
        exit(1);
    }

    c = 0;
    while(c < space_index) {
        cmd[c] = input[c++];
    }
    cmd[c] = 0;

    int cmd_index;

    if(!strcmp(STRDLCK, cmd)) {
        cmd_index = STRDLCK_IND;
    } else if (!strcmp(STWRLCK,cmd)) {
        cmd_index = STWRLCK_IND;
    } else if (!strcmp(PRTLCKS,cmd)) {
        cmd_index = PRTLCKS_IND;
    } else if (!strcmp(FRELCK,cmd)) {
        cmd_index = FRELCK_IND;
    } else if (!strcmp(RDCAHR,cmd)) {
        cmd_index = RDCAHR_IND;
    } else if (!strcmp(WRTCHR,cmd)) {
        cmd_index = WRTCHR_IND;
    } else {
        printf("Unknown command: %s\n", cmd);
        return NULL;
    }

    int num = atoi(&input[space_index+1]);

    command * cm;
    if((cm = malloc(sizeof(command))) == NULL) {
        printf("Memory allocation error 116\n");
        exit(1);
    }

    cm -> cmd = cmd;
    cm -> cmd_index = cmd_index;
    cm -> num = num;

    return cm;
}

void try_lock(command * cmd) {

    struct flock *check_lock;
    if((check_lock = malloc(sizeof(struct flock))) == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    printf("Entered with index: %d\n", cmd -> cmd_index);

    short flock_ltype;

    switch (cmd -> cmd_index) {
        case STRDLCK_IND:
            flock_ltype = F_RDLCK;
            break;
        case STWRLCK_IND:
            flock_ltype = F_WRLCK;
            break;
        case PRTLCKS_IND:
            print_locks();
            return;
        case FRELCK_IND:
            flock_ltype = F_WRLCK;
            break;
        case RDCAHR_IND:
            flock_ltype = F_RDLCK;
            break;
        case WRTCHR_IND:
            flock_ltype = F_WRLCK;
            break;
    }

    check_lock -> l_type = flock_ltype;
    check_lock -> l_start = cmd -> num;
    check_lock -> l_whence = SEEK_SET;
    check_lock -> l_len = 1;
    check_lock -> l_pid = getpid();

    if(fcntl(sys_file_descriptor, F_GETLK, check_lock) == -1) {
//        const char  *msg = explain_fcntl(sys_file_descriptor, F_GETLK, (long) check_lock);
        printf("Error occured during checking lock 193:\n%s\n", file_path);
        return;
    }

    if(check_lock -> l_type != F_UNLCK) {

        printf("Cannot acquire lock, because it is owned. Info:\n");
        print_lock_info(check_lock);

        exit(1);
    } else {
        printf("Debug: I can acquire that lock!\n");
    }

    struct flock *get_lock = check_lock;
    check_lock = NULL;

    switch (cmd -> cmd_index) {

        // try to acquire read lock
        case STRDLCK_IND:

            get_lock -> l_type = F_RDLCK;
            get_lock -> l_start = cmd -> num;
            get_lock -> l_whence = SEEK_SET;
            get_lock -> l_len = 1;
            get_lock -> l_pid = getpid();
            printf("Try to acquire read lock\n");
            int e;
            if((e = fcntl(sys_file_descriptor, F_SETLK, get_lock)) == -1) {
                printf("Err: %d\n", e);
                printf("Error occured during acquiring lock 218\n");
                print_lock_info(get_lock);
                return;
            }
            printf("Succeeded\n");
            break;

        // try to acquire write lock
        case STWRLCK_IND:

            get_lock -> l_type = F_WRLCK;
            get_lock -> l_start = cmd -> num;
            get_lock -> l_whence = SEEK_SET;
            get_lock -> l_len = 1;
            if(fcntl(sys_file_descriptor, F_GETLK, check_lock) == -1) {
                printf("Error occured during acquiring  lock 231\n");
                return;
            }
            break;

        // print locks for this file
        case PRTLCKS_IND:
            print_locks();
            return;

        // free lock
        case FRELCK_IND:
            flock_ltype = F_WRLCK;
            break;

        // read character from this file
        case RDCAHR_IND:
            flock_ltype = F_RDLCK;
            break;

        // write character to this file
        case WRTCHR_IND:
            flock_ltype = F_WRLCK;
            break;
    }
}

void print_locks() {

    printf("Entered print locks...\n");

    struct flock *check_lock;
    if((check_lock = malloc(sizeof(struct flock))) == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    struct stat stat_buf;
    if(lstat(file_path, &stat_buf) != 0) {
        printf("Something went wrong during reading %s's attributes, exit now\n", file_path);
        exit(1);
    } else printf("Readed lstat\n");

    int file_size = (int) stat_buf.st_size;
    int counter = 0;

    int num_locks_printed = 0;

    // iterate over each single byte and print lock info (if any)
    for(counter = 0; counter < file_size; ++counter) {

        printf("print locks iterator %d/%d\n", counter+1, file_size);

        check_lock -> l_type = F_WRLCK;
        check_lock -> l_start = counter;
        check_lock -> l_whence = SEEK_SET;
        check_lock -> l_len = 1;
        if(fcntl(sys_file_descriptor, F_GETLK, check_lock) == -1) {
            printf("Error occured during checking lock 285\n");
            return;
        }

        if(check_lock -> l_type != F_UNLCK) {
            printf("%d Lock found:\n", ++num_locks_printed);
            print_lock_info(check_lock);
        }
    }

}

void print_lock_info(struct flock * lock) {

    char * ctype;
    char * cwhence;

    switch( lock -> l_type ) {
        case F_RDLCK:
            ctype = "F_RDLCK";
            break;
        case F_WRLCK:
            ctype = "F_WRLCK";
            break;
        case F_UNLCK:
            ctype = "F_UNLCK";
            break;
        default:
            break;
    }

    switch( lock -> l_whence ) {
        case SEEK_SET:
            cwhence = "SEEK_SET";
            break;
        case SEEK_CUR:
            cwhence = "SEEK_CUR";
            break;
        case SEEK_END:
            cwhence = "SEEK_END";
            break;
        default:
            break;
    }

    printf("\tl_type: %s\n", ctype);
    printf("\tl_start: %ld\n", lock -> l_start);
    printf("\tl_whence: %s\n", cwhence);
    printf("\tl_len: %ld\n", lock -> l_len);
    printf("\tl_pid: %d\n\n", lock -> l_pid);

}