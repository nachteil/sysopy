#include "file_tree_crawler_ftw.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <ftw.h>
#include <time.h>

char *permit_word;

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
        printf("Memory allocation error 28.\n");
        exit(1);
    }
    strcpy(permit_word, argv[2]);

    __ftw_func_t fun = (__ftw_func_t) &fn;
    if(ftw(argv[1], fun, 5) != 0) {
        printf("FTW operation finished with error.\n");
        exit(1);
    }

    printf("\n");
    return 0;
}

int fn(char * fpath, struct stat *stat_buf, int typeflag) {

//    printf("Enter %s\n", fpath);

    int lvl = 0;
    int counter = 0;
    char c;
    int last_slash_index = -1;
    while((c = fpath[counter++]) != 0) {
        if(c == '/') {
            ++lvl;
            last_slash_index = counter-1;
        }
    }
    int i = lvl;
    int fpath_len = strlen(fpath);
    if(last_slash_index != -1) {
        fpath_len = fpath_len - last_slash_index - 1;
    }
    char * curr_file_name;
//    printf("fpath_len: %d\n", fpath_len);
    if((curr_file_name = malloc(fpath_len + 1)) == NULL) {
        printf("Memory allocation error.\n");
        return -1;
    }

    strcpy(curr_file_name, &fpath[last_slash_index+1]);

    if(S_ISDIR(stat_buf -> st_mode)) {
        i = lvl;
        while(i--) printf("..");
        printf("%s/\n", curr_file_name);

    } else if (S_ISREG(stat_buf -> st_mode) && compare_access(stat_buf)) {
        i = lvl;
        while(i--) printf("..");
        printf("%s, ", curr_file_name);
        printf("%d B, ", (int) stat_buf -> st_size);
        time_t secs = time(NULL);
        struct tm *t = gmtime(&secs);
        printf("%d.%d.%d, %d:%d\n", t -> tm_mday, t -> tm_mon, t -> tm_year + 1900, t -> tm_hour, t -> tm_min);
    }

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