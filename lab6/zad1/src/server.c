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
#include "string.h"

int num_clients = 0;
int clients_queue_ids[MAX_CLIENTS];
int main_queue_id = -1;

int run = 1;

int main() {

    atexit(cleanup);

    if (0 > open(GLOBAL_QUEUE_KEY_FILE, O_CREAT | O_EXCL, S_IRWXU)) {
        error("Server init error (already exists?)");
    }

    key_t global_queue_key = ftok(GLOBAL_QUEUE_KEY_FILE, EX_NUMBER);
    int global_queue_id;
    if ((global_queue_id = create_queue(global_queue_key)) == -1) {
        error("Cannot open server queue");
    }

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients_queue_ids[i] = EMPTY_CLIENT_SLOT;
    }
    main_queue_id = global_queue_id;

    spawn_thread(&client_accepting_function, (void *) &global_queue_id);
    spawn_thread(&enter_command_loop, NULL);

    enter_communicator_loop();

    exit(0);
}

int client_accepting_function(void *argptr) {

    int init_queue_id = *((int *) argptr);
    my_msg_buf msg_buf;

    printf("Accepting thread has been created\n");
    fflush(stdout);

    while (run) {

        msgrcv(init_queue_id, &msg_buf, MSG_SIZE_PARAM, INIT_MSG_TYPE, 0);
        char *unique_id_file = get_client_id_file(msg_buf.client_id);
        printf("Received message from %s\nId file: %s\n", msg_buf.client_id, unique_id_file);

        if (num_clients == MAX_CLIENTS) {
            printf("Too many clients - cannot accept a new one\n");
            continue;
        }

        if (0 > open(unique_id_file, O_CREAT | O_EXCL, S_IRWXU)) {
            printf("Error - cannot create lock for client with id %s\n", msg_buf.client_id);
        } else {
            key_t client_queue_key;
            key_t client_queue_id;

            client_queue_key = ftok(unique_id_file, EX_NUMBER);
            if ((client_queue_id = create_queue(client_queue_key)) == -1) {
                printf("Could not create queue for client id %s\n", msg_buf.client_id);
            } else {
                ++num_clients;
                int i = 0;
                while (clients_queue_ids[i] != EMPTY_CLIENT_SLOT) {
                    ++i;
                }
                clients_queue_ids[i] = client_queue_id;
            }
        }
    }
    return 0;
}

void cleanup(void) {
    run = 0;
    my_msg_buf out_msg_buf;
    strcpy((char*) &(out_msg_buf.msg_body), "exit");
    out_msg_buf.msg_type = S2C;
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        if(clients_queue_ids[i] != EMPTY_CLIENT_SLOT) {
            send_message(clients_queue_ids[i], &out_msg_buf);
        }
    }

    sleep(1);
    printf("Shutdown hook - removing tmp files...\n");
    if (access(GLOBAL_QUEUE_KEY_FILE, F_OK) != -1) {
        if (remove(GLOBAL_QUEUE_KEY_FILE) != 0) {
            printf("Server clean-up error");
        }
    }
    if (main_queue_id != -1) {
        msgctl(main_queue_id, IPC_RMID, NULL);
    }
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients_queue_ids[i] != EMPTY_CLIENT_SLOT) {
            msgctl(clients_queue_ids[i], IPC_RMID, NULL);
        }
    }
    fseek(stdout,0,SEEK_END);
}

void enter_communicator_loop() {

    my_msg_buf in_msg_buf;
    my_msg_buf out_msg_buf;

    while (run) {

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (-1 != receive_message_nonblock(clients_queue_ids[i], &in_msg_buf)) {

                if (!strcmp("exit\n", in_msg_buf.msg_body)) {
                    msgctl(clients_queue_ids[i], IPC_RMID, NULL);
                    printf("%s closed connection\n", in_msg_buf.client_id);
                    clients_queue_ids[i] = EMPTY_CLIENT_SLOT;
                    continue;
                }

                strcpy((char *) &(out_msg_buf.client_id), in_msg_buf.client_id);
                strcpy((char *) &(out_msg_buf.timestamp), in_msg_buf.timestamp);
                strcpy((char *) &(out_msg_buf.msg_body), in_msg_buf.msg_body);
                out_msg_buf.msg_type = S2C;

                for (int j = 0; j < MAX_CLIENTS; ++j) {
                    if (i != j && clients_queue_ids[j] != EMPTY_CLIENT_SLOT) {
                        printf("Get message from %d, passing to %d\n", i, j);
                        send_message(clients_queue_ids[j], &out_msg_buf);
                    }
                }
            }
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