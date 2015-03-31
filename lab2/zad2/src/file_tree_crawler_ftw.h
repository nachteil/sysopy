#ifndef _SYSOPY_FILE_TREE_CRAWLER_FTW_H_
#define _SYSOPY_FILE_TREE_CRAWLER_FTW_H_

#include <sys/stat.h>


    int fn(char *, struct stat *, int);
    int compare_access(struct stat *);
    void verify_access_template(char *);

#endif //_SYSOPY_FILE_TREE_CRAWLER_FTW_H_
