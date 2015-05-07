#ifndef SYSOPY_COMMUNICATION_H
#define SYSOPY_COMMUNICATION_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "common.h"

#define GLOBAL_QUEUE_KEY_FILE "/tmp/jdabrowa-sysopy-lab6-main-queue"
#define CLIENT_UNIQUE_ID_PREFIX "/tmp/jdabrowa-sysopy-lab6-client-"

#define EX_NUMBER 1

#define INIT_MSG_TYPE 1
#define COMMUNICATE_MSG_TYPE 2
#define EXIT_MSG_TYPE 3
#define C2S 7
#define S2C 12
#define MSG_SIZE_PARAM (sizeof(my_msg_buf) - sizeof(long))
#define ANY_MESSAGE_ID 0

typedef struct my_msg_buf {

    long msg_type;
    char client_id[21];
    char timestamp[9];
    char msg_body[1000];

} my_msg_buf;

char *get_client_id_file(char *);

int open_queue(key_t);
int create_queue(key_t);
int send_message( int , my_msg_buf* );

void send_init_msg(int, char *);
int send_exit_msg();

void receive_message_block(int, my_msg_buf*);
int receive_message_nonblock(int, my_msg_buf*);

#endif //SYSOPY_COMMUNICATION_H
