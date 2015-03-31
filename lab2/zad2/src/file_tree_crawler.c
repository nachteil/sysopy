#include "file_tree_crawler.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>

int path_buffer_size;
int indentation_lvl = 0;

char * path_buffer;

char * permit_word;

int main(int argc, char * argv[]) {

    if(argc != 3) {
        printf("You must specify a root path and access rights template.\n");
        exit(1);
    }

    if(access(argv[1], F_OK) != 0) {
        printf("Cannot open specified path.\n");
        exit(1);
    }

    verify_access_template(argv[2]);
    if((permit_word = malloc(10)) == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    strcpy(permit_word, argv[2]);

    char * root_path = argv[1];
    path_buffer = get_path_buffer();
    printf("Length: %d\n", path_buffer_size);
    if(path_buffer_size < strlen(root_path)) {
        path_buffer_size *= 2;
        if((path_buffer = realloc(path_buffer, path_buffer_size)) == NULL) {
            printf("Main: emory allocation error.\n");
            exit(1);
        }
    }

    strcpy(path_buffer, root_path);

    traverse_recursively();

    return 0;
}


void verify_access_template(char *templ) {

    if(strlen(templ) != 9) {
        printf("Template should have length 9\n");
    }

    char *t = "rwx";

    int counter = 0;
    for(counter = 0; counter < 9; ++counter) {
        printf("templ[%d] = %c, t[%d] = %c, templ[%d] = %c\n", counter, templ[counter], counter%3, t[counter%3], counter, templ[counter]);
        if(templ[counter] != t[counter % 3] && templ[counter] != '-') {
            printf("Template should be in standard unix form: rwxrwxrwx (with dash as no access)\n");
            exit(1);
        }
    }
}



char *get_path_buffer() {

    int max_len = pathconf("/", _PC_PATH_MAX);

    printf("Max_len: %d\n", max_len);

    if (max_len <= 0) {
        max_len = 2048;
    } else {
        max_len += 2;
    }

    char * buffer;
    if ((buffer = malloc(max_len)) == NULL) {
        printf("Get_path_buffer: Memory allocation error\n");
    }

    path_buffer_size = max_len;
    printf("Max len: %d\n", max_len);
    return buffer;
}


void traverse_recursively() {

    // we only enter here if we're sure that path_buffer points to a directory

//    printf("Entering: %s\n", path_buffer);

    DIR * current_dir;
    if((current_dir = opendir(path_buffer)) == NULL) {
        printf("Something went wrong in %s, won't go further\n", path_buffer);
    }

    struct dirent *currently_viewed_file;
    while((currently_viewed_file = readdir(current_dir)) != NULL) {
        char * curr_file_name = currently_viewed_file -> d_name;
        if(strcmp(curr_file_name, ".") && strcmp(curr_file_name, "..")) {

            int path_len = strlen(path_buffer);
            while(path_len + strlen(curr_file_name) + 2 > path_buffer_size) {
                path_buffer_size *= 2;
                if((path_buffer = realloc(path_buffer, path_buffer_size)) == NULL) {
                    printf("Recursive: Memory allocation error\n");
                    exit(1);
                }
            }

            path_buffer[path_len++] = '/';
            strcpy(&path_buffer[path_len], curr_file_name);
            int i = indentation_lvl;

            struct stat stat_buf;
            if(lstat(path_buffer, &stat_buf) != 0) {
                printf("Something went wrong in %s, won't go further\n", curr_file_name);
                return;
            }
            if(S_ISDIR(stat_buf.st_mode)) {
//                printf("%s is a dir\n", curr_file_name);

                while(i--) printf("..");
                printf("%s/\n", curr_file_name);
                ++indentation_lvl;
                traverse_recursively(path_buffer);
                --indentation_lvl;

            } else if (S_ISREG(stat_buf.st_mode) && compare_access(&stat_buf)) {
                int i = indentation_lvl;
                while(i--) printf("..");
                printf("%s, ", curr_file_name);
                printf("%d B, ", (int) stat_buf.st_size);
                time_t secs = time(NULL);
                struct tm *t = gmtime(&secs);
                printf("%d.%d.%d, %d:%d\n", t -> tm_mday, t -> tm_mon, t -> tm_year + 1900, t -> tm_hour, t -> tm_min);
            }

            path_buffer[--path_len] = 0;
        }
    }

}

int compare_access(struct stat *stat_buf) {

    if(S_IRUSR & stat_buf->st_mode)
        if(permit_word[0] != 'r')
            return 0;

    if(S_IWUSR & stat_buf->st_mode)
        if(permit_word[1] != 'w')
            return 0;

    if(S_IXUSR & stat_buf->st_mode)
        if(permit_word[2] != 'x')
            return 0;

    if(S_IRGRP & stat_buf->st_mode)
        if(permit_word[3] != 'r')
            return 0;

    if(S_IWGRP & stat_buf->st_mode)
        if(permit_word[4] != 'w')
            return 0;

    if(S_IXGRP & stat_buf->st_mode)
        if(permit_word[5] != 'x')
            return 0;

    if(S_IROTH & stat_buf->st_mode)
        if(permit_word[6] != 'r')
            return 0;

    if(S_IWOTH & stat_buf->st_mode)
        if(permit_word[7] != 'w')
            return 0;

    if(S_IXOTH & stat_buf->st_mode)
        if(permit_word[8] != 'x')
            return 0;

    return 1;
}