#include "file_counter.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "dirent.h"
#include "errno.h"
#include "string.h"
#include "sys/wait.h"

int main(int argc, char *argv[]) {

    int test_mode = 0;
    int should_log = 0;
    char *path;

    if (argc == 4) {
        if ((!strcmp(argv[1], "-v") && !strcmp(argv[2], "-w"))
            || (!strcmp(argv[1], "-w") && !strcmp(argv[2], "-v"))) {
            test_mode = should_log = 1;
        } else {
            usage();
        }
    } else if (argc == 3) {
        if (!strcmp(argv[1], "-wv") || !strcmp(argv[1], "-vw")) {
            test_mode = 1;
            should_log = 1;
        } else if (!strcmp(argv[1], "-v")) {
            should_log = 1;
        } else if (!strcmp(argv[1], "-w")) {
            test_mode = 1;
        } else {
            usage();
        }
    } else if (argc > 4 || argc < 2) {
        usage();
    }

    if ((path = (char *) malloc(1024)) == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    strcpy(path, argv[argc - 1]);

    // actual code starts here

    DIR *dir;
    struct dirent *dir_entry;
    int num_processes = 0;
    long pid;
    int status;
    int num_files = 0;

    if ((dir = opendir(path)) == NULL) {
        printf("%s is not a dir!\n", path);
        exit(-1);
    }


    while ((dir_entry = readdir(dir)) != NULL) {
        char *curr_file_name = dir_entry->d_name;

        if (strcmp(curr_file_name, ".") && strcmp(curr_file_name, "..")) {

            int path_len = strlen(path);

            path[path_len++] = '/';
            strcpy(&path[path_len], curr_file_name);

            struct stat stat_buf;
            if (lstat(path, &stat_buf) != 0) {
                printf("Something went wrong in %s reading %s, won't go further\n", path, curr_file_name);
                printf("Error: %d (%s)\n", errno, strerror(errno));
                exit(-1);
            }

            if (S_ISDIR(stat_buf.st_mode)) {

                num_processes += 1;
                if ((pid = fork()) < 0) {

                    printf("Forking error at %s\n", path);
                    printf("Error message: %d - %s\n", errno, strerror(errno));
                    exit(-1);

                } else if (pid == 0) {
                    errno = 0;
                    if (should_log || test_mode) {
                        char opts[3];
                        opts[0] = '-';
                        opts[1] = should_log ? 'v' : 'w';
                        opts[2] = test_mode && should_log ? 'w' : 0;
                        execl(argv[0], argv[0], opts, path, (char *) 0);
                        if (errno != 0) {
                            printf("Error on execl: %d - %s\n", errno, strerror(errno));
                        }
                    } else {
                        execl(argv[0], argv[0], path, (char *) 0);
                        if (errno != 0) {
                            printf("Error on execl: %d - %s\n", errno, strerror(errno));
                        }
                    }
                }

            } else if (S_ISREG(stat_buf.st_mode)) {
                ++num_files;
            }
            path[--path_len] = 0;
        }
    }
    closedir(dir);

    if (test_mode) {
        sleep(10);
    }

    int child_files = 0;
    while (num_processes--) {
        wait(&status);
        if (WEXITSTATUS(status) == -1) {
            exit(-1);
        } else {
            child_files += WEXITSTATUS(status);
        }
    }

    if (should_log) {
        printf("%s, %d, %d\n", path, num_files, num_files + child_files);
    }

    exit(num_files + child_files);
}

void usage(void) {
    printf("Wrong program arguments\n");
    printf("Usage: file_counter <options> path, where <options> is any combination of the following:\n");
    printf("\t-w - to enter test mode (make each process sleep for 10 seconds before terminating)\n");
    printf("\t-v - enable logging (verbose mode)\n");
    exit(1);
}