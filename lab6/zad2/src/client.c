#define _GNU_SOURCE

#include "client.h"
#include "communication.h"
#include "common.h"
#include "unistd.h"
#include "string.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sched.h"
#include "stdlib.h"
#include "stdio.h"
#include "errno.h"

int run = 1;
char *line_buffer;
char client_id[22];

int main(int argc, char * argv[]) {

    if(argc != 2) {
        error("Expecting one argument - unique identifier");
    }

    if(strlen(argv[1]) > 20) {
        error("Identifier should be 20 characters or less");
    }
    char *c = argv[1];
    while(*c != 0) {
        if(*(c++) == '/') {
            error("Identifier cannot contain slashes");
        }
    }

    client_id[0] = '/';
    strcpy(&(client_id[1]), argv[1]);

    if((line_buffer = (char *)malloc(1024)) == NULL) {
        mem_error();
    }

    if(mq_open(client_id, O_RDONLY) != (mqd_t) -1 || errno !=  ENOENT) {
        perror("Client with this name exists\n");
        exit(-1);
    }

    mqd_t global_queue_id;
    if((global_queue_id = open_queue(MAIN_QUEUE_NAME)) == (mqd_t)-1) {
        if(errno == ENOENT) {
            error("Server queue not found");
        }
        perror(NULL);
        error("Cannot open server queue");
    }
    send_init_msg(global_queue_id, client_id);


    int timeout = 5;
    int count = 0;
    // open client queue
    mqd_t client_queue_id;
    while(count < timeout && (client_queue_id = open_queue(client_id)) == (mqd_t)-1) {
        ++count;
        sleep(1);
    }
    if(count == timeout) {
        error("Timeout - couldn't open queue");
    }

    fflush(stdout);
    spawn_thread(receiver_loop, (void *) &client_queue_id);

    sender_loop(client_id, global_queue_id);

    exit(0);
}

int receiver_loop(void * argptr) {

    mqd_t queue_id = *((mqd_t *) argptr);
    my_msg_buf msg_buf;

    while(run) {
        receive_message_block(queue_id, &msg_buf);
        if(!strcmp("exit", msg_buf.msg_body)) {
            reset_line(line_buffer, 1024);
            sprintf(line_buffer, "Server closed connection\n");
            write(STDOUT_FILENO, line_buffer, strlen(line_buffer));
            exit(0);
        }
        reset_line(line_buffer, 1024);
        sprintf(line_buffer, "%s - %s\n%s\n", &(msg_buf.client_id[1]), msg_buf.timestamp, msg_buf.msg_body);
        write(STDOUT_FILENO, line_buffer, strlen(line_buffer));
    }

    exit(0);
}

int sender_loop(char *client_id, mqd_t client_queue_id) {

    int buf_size = 1000;
    char *msg_text_buff = (char *) malloc(buf_size);
    if(msg_text_buff == NULL) {
        mem_error();
    }
    my_msg_buf msg_buff_struct;
    strcpy((char *) &(msg_buff_struct.client_id), client_id);

    while(run) {
        fgets(msg_text_buff, buf_size, stdin);
        char *timestamp = get_formatted_hour();
        strcpy((char *) &(msg_buff_struct.timestamp), timestamp);
        free(timestamp);
        strcpy((char *) &(msg_buff_struct.msg_body), msg_text_buff);
        send_message(client_queue_id, &msg_buff_struct);
        write(STDOUT_FILENO, "\n", 1);
        if(!strcmp(msg_text_buff, "exit\n")) {
            exit(0);
        }
    }
    return 0;
}
