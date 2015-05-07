#define _GNU_SOURCE

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include "unistd.h"
#include "common.h"
#include "communication.h"
#include "stdlib.h"
#include "errno.h"
#include "mqueue.h"
#include "string.h"

int num_clients = 0;
mqd_t clients_queue_ids[MAX_CLIENTS];
char *client_names[MAX_CLIENTS];

mqd_t main_queue_id = (mqd_t) - 1;

int run = 1;

int main() {

    atexit(cleanup);

    mqd_t global_queue_id;
    if ((global_queue_id = create_queue(MAIN_QUEUE_NAME)) == -1) {
        if (errno == EEXIST) {
            error("Server queue already exists");
        }
        error("Cannot create server queue");
    }

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients_queue_ids[i] = (mqd_t) EMPTY_CLIENT_SLOT;
    }
    main_queue_id = global_queue_id;

    spawn_thread(&enter_command_loop, NULL);

    enter_communicator_loop();

    exit(0);
}

void cleanup(void) {
    run = 0;
    my_msg_buf out_msg_buf;
    strcpy((char *) &(out_msg_buf.msg_body), "exit");
    out_msg_buf.msg_type = MSG_TYPE_TEXT;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients_queue_ids[i] != EMPTY_CLIENT_SLOT) {
            send_message(clients_queue_ids[i], &out_msg_buf);
        }
    }

    sleep(1);
    printf("Shutdown hook - removing tmp files...\n");
    if (main_queue_id != (mqd_t) - 1) {
        mq_unlink(MAIN_QUEUE_NAME);
    }
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients_queue_ids[i] != (mqd_t) EMPTY_CLIENT_SLOT) {
            mq_unlink(client_names[i]);
        }
    }
    fseek(stdout, 0, SEEK_END);
}

void enter_communicator_loop() {

    my_msg_buf in_msg_buf;
    my_msg_buf out_msg_buf;

    printf("Entered server communication loop\n");

    while (run) {
        if ((mqd_t) - 1 != receive_message_block(main_queue_id, &in_msg_buf)) {

            if (in_msg_buf.msg_type == MSG_TYPE_INIT) {

                printf("Received init message from %s\n", in_msg_buf.client_id);

                if (num_clients == MAX_CLIENTS) {
                    printf("Too many clients - cannot accept a new one\n");
                    continue;
                }

                mqd_t client_queue_id;

                if ((client_queue_id = create_queue(in_msg_buf.client_id)) == (mqd_t) - 1) {
                    printf("Could not create queue for client id %s\n", in_msg_buf.client_id);
                } else {
                    ++num_clients;
                    int i = 0;
                    while (clients_queue_ids[i] != (mqd_t) EMPTY_CLIENT_SLOT) {
                        ++i;
                    }
                    clients_queue_ids[i] = client_queue_id;
                    if ((client_names[i] = (char *) malloc(21)) == NULL) {
                        mem_error();
                    }
                    strcpy(client_names[i], in_msg_buf.client_id);
                }

            } else {

                if (!strcmp("exit\n", in_msg_buf.msg_body)) {
                    mq_unlink(in_msg_buf.client_id);
                    printf("%s closed connection\n", in_msg_buf.client_id);
                    int i = 0;
                    while (i < MAX_CLIENTS && (client_names[i] == NULL || strcmp(client_names[i], in_msg_buf.client_id))) ++i;
                    if (i < MAX_CLIENTS) {
                        clients_queue_ids[i] = EMPTY_CLIENT_SLOT;
                        client_names[i] = NULL;
                    }
                    continue;
                }

                strcpy((char *) &(out_msg_buf.client_id), in_msg_buf.client_id);
                strcpy((char *) &(out_msg_buf.timestamp), in_msg_buf.timestamp);
                strcpy((char *) &(out_msg_buf.msg_body), in_msg_buf.msg_body);
                out_msg_buf.msg_type = MSG_TYPE_TEXT;

                for (int j = 0; j < MAX_CLIENTS; ++j) {
                    if (clients_queue_ids[j] != (mqd_t) EMPTY_CLIENT_SLOT &&
                        strcmp(client_names[j], in_msg_buf.client_id)) {
                        printf("Get message from %s, passing to %d\n", in_msg_buf.client_id, j);
                        send_message(clients_queue_ids[j], &out_msg_buf);
                    }
                }
            }
        } else {
            perror("Error en message receive in server\n");
        }
    }

}

int enter_command_loop(void *argptr) {

    int buf_size = 1000;
    char *msg_text_buff = (char *) malloc(buf_size);
    if (msg_text_buff == NULL) {
        mem_error();
    }

    while (run) {

        fgets(msg_text_buff, buf_size, stdin);
        if (!strcmp(msg_text_buff, "exit\n")) {
            exit(0);
        } else {
            printf("Uknown command: %s\n", msg_text_buff);
        }
    }
    return 0;
}