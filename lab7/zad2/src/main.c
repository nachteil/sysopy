#include "main.h"
#include "stdio.h"
#include "common.h"

int main(int argc, char * argv) {

    int d;
    while(1) {
        scanf("%d", &d);
        printf("%s\n", get_timestamp_with_millis());
    }

    return 0;
}