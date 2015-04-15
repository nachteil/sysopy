#define _GNU_SOURCE
#include "main.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include <dirent.h>
#include "string.h"

// ls -l | grep ^d | wc -l

int main(int argc, char * argv[]) {

    char cwd[2048];
    getcwd(cwd, 2048);
    if(cwd == NULL) {
        printf("Memory allocation error\n");
        exit(-1);
    }

    printf("%s\n", cwd);



    // prepare and fork to grep
    int fd[2];
    int pid;

    if(pipe(fd) < 0) {
        printf("pipe_error\n");
    }

    if((pid = fork()) < 0) {
        printf("fork error\n");
    } else if (pid > 0) {   // parent process
        close(fd[0]);

        DIR * basedir;
        if((basedir = opendir(cwd)) == NULL) {
            printf("Something went wrong in %s, won't go further\n", cwd);
        }

        struct dirent *dir_entry;
        while((dir_entry = readdir(basedir)) != NULL) {

            char * curr_file_name = dir_entry -> d_name;
            int path_len = strlen(cwd);
            cwd[path_len++] = '/';
            strcpy(&cwd[path_len], curr_file_name);

            struct stat stat_buf;
            if(lstat(cwd, &stat_buf) != 0) {
                printf("Something went wrong in %s, won't go further\n", curr_file_name);
                exit(-1);
            }

            char *access = get_access(&stat_buf);
            printf("%s %s\n", access, curr_file_name);
            cwd[--path_len] = 0;

            char *line = (char*) malloc(1024*sizeof(char));
            if(line == NULL) {
                printf("Allocation error\n");
            }
            // sprawdzic polecenie powloki strace
            strcpy(line, access);
            line[10] = ' ';
            char size[100];
            sprintf(size, "%d", (int)stat_buf.st_size);
            strcpy(&line[11], size);
            int len = strlen(line);
            strcpy(&line[len], curr_file_name);
            len = strlen(line);
            line[len] = '\n';
            line[len+1] = 0;

            write(fd[1], access, len+2);
        }

        close(fd[1]);
        exit(0);

    } else if (pid == 0) {  // child process
        close(fd[1]);
        char *line = (char*) malloc(512);
        if(line == NULL) {
            printf("allocation error\n");
        }

        while(read(fd[0], line, 512)) {
            printf("%s\n", line);
        }

        close(fd[0]);
        exit(0);
    }

    exit(0);
}

char *get_access(struct stat *stat_buf) {

    char *access;
    if((access = (char*) malloc(10* sizeof(char))) == NULL) {
        printf("Allocation error\n");
        exit(-1);
    }

    if(S_ISDIR(stat_buf->st_mode)) {
        access[0] = 'd';
    } else {
        access[0] = '-';
    }

    access[1] = (S_IRUSR & stat_buf->st_mode) ? 'r' : '-';
    access[2] = (S_IWUSR & stat_buf->st_mode) ? 'w' : '-';
    access[3] = (S_IXUSR & stat_buf->st_mode) ? 'x' : '-';
    access[4] = (S_IRGRP & stat_buf->st_mode) ? 'r' : '-';
    access[5] = (S_IWGRP & stat_buf->st_mode) ? 'w' : '-';
    access[6] = (S_IXGRP & stat_buf->st_mode) ? 'x' : '-';
    access[7] = (S_IROTH & stat_buf->st_mode) ? 'r' : '-';
    access[8] = (S_IWOTH & stat_buf->st_mode) ? 'w' : '-';
    access[9] = (S_IXOTH & stat_buf->st_mode) ? 'x' : '-';

    return access;
}