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

char *unique_id_file = NULL;

int run = 1;
char *line_buffer;

int main(int argc, char * argv[]) {

    if(argc != 2) {
        error("Expecting one argument - unique identifier");
    }

    if(strlen(argv[1]) > 20) {
        error("Identifier should be 20 characters or less");
    }

    atexit(cleanup_fn);

    unique_id_file = get_client_id_file(argv[1]);

    if(access(unique_id_file, F_OK) != -1) {
        error("This client id as already used");
    }

    if((line_buffer = (char *)malloc(1024)) == NULL) {
        mem_error();
    }

    // find server and init
    if( access( GLOBAL_QUEUE_KEY_FILE, F_OK ) == -1 ) {
        error("Server not found, cannot start client");
    }
    key_t global_queue_key = ftok(GLOBAL_QUEUE_KEY_FILE, EX_NUMBER);
    int global_queue_id;
    if((global_queue_id = open_queue(global_queue_key)) == -1) {
        error("Cannot open server queue");
    }
    send_init_msg(global_queue_id, argv[1]);


    // wait until server creates a queue for this client (will signal by creating file)
    // warning - may timeout!
    int client_timeout = 5;
    int timeout_counter = 0;
    do {
        usleep(500);
        if(++timeout_counter == client_timeout) {
            error("Client connection timeout");
        }
    } while(access(unique_id_file, F_OK) == -1);

    // open client queue
    key_t client_queue_key = ftok(unique_id_file, EX_NUMBER);
    int client_queue_id;
    if((client_queue_id = open_queue(client_queue_key)) == -1) {
        error("Cannot open client queue");
    }

    fflush(stdout);
    spawn_thread(receiver_loop, (void *) &client_queue_id);

    sender_loop(argv[1], client_queue_id);

    exit(0);
}

int receiver_loop(void * argptr) {

    int queue_id = *((int *) argptr);
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
        sprintf(line_buffer, "%s - %s\n%s\n", msg_buf.client_id, msg_buf.timestamp, msg_buf.msg_body);
        write(STDOUT_FILENO, line_buffer, strlen(line_buffer));
    }

    exit(0);
}

int sender_loop(char *client_id, int client_queue_id) {

    int buf_size = 1000;
    char *msg_text_buff = (char *) malloc(buf_size);
    if(msg_text_buff == NULL) {
        mem_error();
    }
    my_msg_buf msg_buff_struct;
    strcpy((char *) &(msg_buff_struct.client_id), client_id);
    msg_buff_struct.msg_type = C2S;

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

void cleanup_fn(void) {

    run = 0;
    reset_line(line_buffer, 1024);
    sprintf(line_buffer, "Client shutdown\nRemoving %s\n", unique_id_file);
    write(STDOUT_FILENO, line_buffer, strlen(line_buffer));
    if(remove(unique_id_file) != 0) {
        error("File operation error");
    }
}