#define _GNU_SOURCE

#include "main.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "sys/stat.h"
#include <dirent.h>
#include "string.h"

// ls -l | grep ^d | wc -l

int main(int argc, char *argv[]) {

    char cwd[2048];
    getcwd(cwd, 2048);
    if (cwd == NULL) {
        printf("Memory allocation error\n");
        exit(-1);
    }

    printf("%s\n", cwd);

    // prepare and fork to grep
    int fd[2];
    int pid;

    if (pipe(fd) < 0) {
        printf("pipe_error\n");
    }

    if ((pid = fork()) < 0) {
        printf("fork error\n");
    } else if (pid > 0) {   // parent process [ls -l]
        close(fd[0]);

        DIR *basedir;
        if ((basedir = opendir(cwd)) == NULL) {
            printf("Something went wrong in %s, won't go further\n", cwd);
        }

        struct dirent *dir_entry;

        while ((dir_entry = readdir(basedir)) != NULL) {

            char *curr_file_name = dir_entry->d_name;
            int path_len = strlen(cwd);
            cwd[path_len++] = '/';
            strcpy(&cwd[path_len], curr_file_name);

            struct stat stat_buf;
            if (lstat(cwd, &stat_buf) != 0) {
                printf("Something went wrong in %s, won't go further\n", curr_file_name);
                exit(-1);
            }

            char *access = get_access(&stat_buf);
            cwd[--path_len] = 0;

            char *line = (char *) malloc(1024 * sizeof(char));
            if (line == NULL) {
                printf("Allocation error\n");
            }

            // check how strace shell command works
            strcpy(line, access);
            line[10] = ' ';
            char size[100];
            sprintf(size, "%d", (int) stat_buf.st_size);
            strcpy(&line[11], size);
            int len = strlen(line);
            strcpy(&line[len], curr_file_name);
            len = strlen(line);
            line[len] = '\n';
            line[len + 1] = 0;

            if (strcmp("..", curr_file_name) && strcmp(".", curr_file_name)) {
                write(fd[1], line, strlen(line));
            }
        }

        int status;
        close(fd[1]); // will cause the child to go out of the loop

        wait(&status);
        if (WEXITSTATUS(status) != 0) {
            exit(WEXITSTATUS(status));
        }
        exit(0);

    } else if (pid == 0) {  // child process [ grep ^d | wc -l ]

        close(fd[1]);

        int sub_fd[2];
        if (pipe(sub_fd) < 0) {
            printf("Pipe error\n");
            exit(-1);
        }

        if ((pid = fork()) < 0) {
            printf("Forking error\n");
            exit(-1);
        } else if (pid > 0) {   // parent process [grep ^d]

            close(sub_fd[0]);
            char *line = (char *) malloc(1024);
            if (line == NULL) {
                printf("allocation error\n");
            }

            FILE *in = fdopen(fd[0], "r");

            if (!in) {
                printf("Cannot open file descriptor\n");
                exit(1);
            }

            while (fgets(line, 1024, in)) {
                if (line[0] == 'd') {
                    write(sub_fd[1], line, strlen(line));
                }
            }

            close(sub_fd[1]);
            close(fd[0]);

            fclose(in);

            int status;
            wait(&status);
            if (WEXITSTATUS(status) != 0) {
                exit(WEXITSTATUS(status));
            }

            exit(0);

        } else {                // child process [wc -l]

            close(sub_fd[1]);

            char *wc_line = (char *) malloc(1024);
            if (!wc_line) {
                printf("Memory allocation error\n");
                exit(-1);
            }

            FILE *in = fdopen(sub_fd[0], "r");

            if (!in) {
                printf("Cannot open file descriptor\n");
                exit(-1);
            }

            int num_lines = 0;
            while (fgets(wc_line, 1024, in)) {
                ++num_lines;
            }

            printf("%d\n", num_lines);
            close(sub_fd[0]);
        }


        close(fd[0]);
        exit(0);
    }

    exit(0);
}

char *get_access(struct stat *stat_buf) {

    char *access;
    if ((access = (char *) malloc(10 * sizeof(char))) == NULL) {
        printf("Allocation error\n");
        exit(-1);
    }

    if (S_ISDIR(stat_buf->st_mode)) {
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