#ifndef SYSOPY_COMMUNICATION_H
#define SYSOPY_COMMUNICATION_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mqueue.h"
#include "common.h"

#define MAIN_QUEUE_NAME "/jdabrowa-sysopy-lab6-main-queue"
#define MSG_SIZE_PARAM (sizeof(my_msg_buf))

#define MSG_TYPE_INIT 1
#define MSG_TYPE_TEXT 2

typedef struct my_msg_buf {
    int msg_type;
    char client_id[21];
    char timestamp[9];
    char msg_body[1000];

} my_msg_buf;

char *get_client_id_file(char *);

mqd_t open_queue(char *);
mqd_t create_queue(char *);
int send_message( mqd_t , my_msg_buf* );

void send_init_msg(mqd_t, char *);

int receive_message_block(mqd_t, my_msg_buf*);

#endif //SYSOPY_COMMUNICATION_H
