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
#include <sys/wait.h>
#include <sched.h>


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

    sys_file_descriptor = -1;
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
    printf("\tEnter 'exit' (no quotes) to exit\n");

    char input[256];

    while(1) {
        printf("> ");
        fgets(input, 255, stdin);
        command *cmd = parse(input);
        if(cmd == NULL) continue;
        try_lock(cmd);
    }
}

command *parse(char * input) {

    if(!strcmp("exit", input)) {
        printf("Ending program\n");
        exit(0);
    }

    // remove newline and spaces at the end
    int c = strlen(input) - 1;
    input[c--] = 0;
    while(c >= 0) {
        if(input[c] == ' ' || input[c] == '\t') {
            input[c--] = 0;
        } else {
            break;
        }
    }

    int start_index = 0;
    while(input[start_index] == ' ' || input[start_index] == '\t') ++start_index;

    int num_spaces = 0;
    c = start_index;
    int len = strlen(input);
    int space_index = -1;
    while(c < len) {
        if(input[c++] == ' ') {
            ++num_spaces;
            space_index = c - 1;
        }
    }

    if(num_spaces > 1) {
        printf("Wrong input format\n");
        return NULL;
    }

    char *cmd = malloc(space_index + 1);
    if(cmd == NULL) {
        printf("Memory allocation error\n");
        return NULL;
    }

    c = 0;
    while(c < (num_spaces != 0 ? space_index : len)) {
        cmd[c] = input[start_index + c];
        ++c;
    }

    int cmd_index;
    if(!strcmp(STRDLCK, cmd)) {
        if(num_spaces != 1) {
            printf("Wrong input format\n");
            return NULL;
        }
        cmd_index = STRDLCK_IND;
    } else if (!strcmp(STWRLCK,cmd)) {
        if(num_spaces != 1) {
            printf("Wrong input format\n");
            return NULL;
        }
        cmd_index = STWRLCK_IND;
    } else if (!strcmp(PRTLCKS,cmd)) {
        if(num_spaces != 0) {
            printf("Wrong input format\n");
            return NULL;
        }
        cmd_index = PRTLCKS_IND;
    } else if (!strcmp(FRELCK,cmd)) {
        if(num_spaces != 1) {
            printf("Wrong input format\n");
            return NULL;
        }
        cmd_index = FRELCK_IND;
    } else if (!strcmp(RDCAHR,cmd)) {
        if(num_spaces != 1) {
            printf("Wrong input format\n");
            return NULL;
        }
        cmd_index = RDCAHR_IND;
    } else if (!strcmp(WRTCHR,cmd)) {
        if(num_spaces != 1) {
            printf("Wrong input format\n");
            return NULL;
        }
        cmd_index = WRTCHR_IND;
    } else {
        printf("Unknown command: %s\n", cmd);
        return NULL;
    }

    int num;
    if(num_spaces == 1) {
        num = atoi(&input[space_index+1]);
    } else {
        num = 0;
    }

    command * cm;
    if((cm = malloc(sizeof(command))) == NULL) {
        printf("Memory allocation error\n");
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

    short flock_ltype;
    int fork_id;
    switch (cmd -> cmd_index) {
        case STRDLCK_IND:
            flock_ltype = F_RDLCK;
            break;
        case STWRLCK_IND:
            flock_ltype = F_WRLCK;
            break;
        case PRTLCKS_IND:
            fork_id = fork();
            // use forks to include locks owned by parent process
            if(fork_id == 0) // executed in child process
            {
                print_locks();
                _exit(0);
            } else // executed in parent process
            {
                waitpid(fork_id, NULL, 0);
            }
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
        printf("Error occured during checking lock:\n%s\n", file_path);
        return;
    }

    if(check_lock -> l_type != F_UNLCK) {

        printf("Cannot perform this operation on lock, because it is owned. Info:\n");
        print_lock_info(check_lock);

        return;
    }

    struct flock *get_lock = check_lock;
    check_lock = NULL;
    int bytes_read;
    char byte_buf;
    char double_buf[2];

    switch (cmd -> cmd_index) {

        // try to acquire read lock
        case STRDLCK_IND:

            get_lock -> l_type = F_RDLCK;
            get_lock -> l_start = cmd -> num;
            get_lock -> l_whence = SEEK_SET;
            get_lock -> l_len = 1;
            get_lock -> l_pid = getpid();
            if(fcntl(sys_file_descriptor, F_SETLK, get_lock) == -1) {
                printf("Error occured during acquiring lock 218\n");
                print_lock_info(get_lock);
                return;
            }
            printf("Successfully acquired read lock\n");
            break;

        // try to acquire write lock
        case STWRLCK_IND:

            get_lock -> l_type = F_WRLCK;
            get_lock -> l_start = cmd -> num;
            get_lock -> l_whence = SEEK_SET;
            get_lock -> l_len = 1;
            get_lock -> l_pid = getpid();
            if(fcntl(sys_file_descriptor, F_SETLK, get_lock) == -1) {
                printf("Error occured during acquiring  lock\n");
                return;
            }
            printf("Successfully acquired write lock\n");
            break;

        // free lock
        case FRELCK_IND:

            get_lock -> l_type = F_UNLCK;
            get_lock -> l_start = cmd -> num;
            get_lock -> l_whence = SEEK_SET;
            get_lock -> l_len = 1;
            get_lock -> l_pid = getpid();
            if(fcntl(sys_file_descriptor, F_SETLK, get_lock) == -1) {
                printf("Error occured during acquiring  lock\n");
                return;
            }
            printf("Successfully released lock\n");
            break;

        // read character from this file
        case RDCAHR_IND:

            if(lseek(sys_file_descriptor, cmd -> num, SEEK_SET) == -1)
            {
                printf("File access error.\n");
                return;
            }
            bytes_read = read(sys_file_descriptor, &byte_buf, 1);
            if(bytes_read != 1) {
                printf("File read error\n");
                return;
            }
            printf("Readed character: %c\n", byte_buf);
            break;

        // write character to this file
        case WRTCHR_IND:

            if(lseek(sys_file_descriptor, cmd -> num, SEEK_SET) == -1) {
                printf("File access error.\n");
                return;
            }

            printf("Enter the character to place at position %d:\n", cmd -> num);
            fgets(double_buf, 2, stdin);

            if(write(sys_file_descriptor, &double_buf[0], 1) == -1) {
                printf("File write error.\n");
                return;
            }

            printf("Successfully writen '%c' at index %d\n", double_buf[0], cmd -> num);
            break;
    }
}

void print_locks() {


    struct flock *check_lock;
    if((check_lock = malloc(sizeof(struct flock))) == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    struct stat stat_buf;
    if(lstat(file_path, &stat_buf) != 0) {
        printf("Something went wrong during reading %s's attributes, exit now\n", file_path);
        exit(1);
    }

    int file_size = (int) stat_buf.st_size;
    int counter = 0;

    int num_locks_printed = 0;

    // iterate over each single byte and print lock info (if any)
    for(counter = 0; counter < file_size; ++counter) {

        check_lock -> l_type = F_WRLCK;
        check_lock -> l_start = counter;
        check_lock -> l_whence = SEEK_SET;
        check_lock -> l_len = 1;
        check_lock -> l_pid = getpid();
        if(fcntl(sys_file_descriptor, F_GETLK, check_lock) == -1) {
            printf("Error occured during checking lock\n");
            return;
        }
        if(check_lock -> l_type != F_UNLCK) {
            printf("%d.found lock:\n", ++num_locks_printed);
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

    printf("\tl_type:     %s\n", ctype);
    printf("\tl_start:    %ld\n", lock -> l_start);
    printf("\tl_whence:   %s\n", cwhence);
    printf("\tl_len:      %ld\n", lock -> l_len);
    printf("\tl_pid:      %d\n\n", lock -> l_pid);

}