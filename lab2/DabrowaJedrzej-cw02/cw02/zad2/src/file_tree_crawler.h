#ifndef _SYSOPY_FILE_TREE_CRAWLER_H_
#define _SYSOPY_FILE_TREE_CRAWLER_H_
#include <sys/stat.h>

void verify_access_template(char *);
char * get_path_buffer();
void traverse_recursively();
int compare_access(struct stat *);

#endif //_SYSOPY_FILE_TREE_CRAWLER_H_
