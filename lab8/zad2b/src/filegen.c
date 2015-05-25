#define _GNU_SOURCE

#include "filegen.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#define REC_SIZE 1024

int main(int argc, char * argv[]) {

    int num_recs = atoi(argv[1]);
    char *fname = argv[2];

    int filedes = open(fname, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    char tab[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()";

    srand(time(NULL));

    char *rec_buf = malloc(REC_SIZE- sizeof(int));

    for(int i = 0; i < num_recs; ++i) {
        write(filedes, (unsigned char *)(&i), sizeof(int));
        for(int j = 0; j < REC_SIZE - sizeof(int); ++j) {
            rec_buf[j] = tab[rand() % 72];
        }
        write(filedes, rec_buf, REC_SIZE - sizeof(int));
    }

    exit(0);
}
