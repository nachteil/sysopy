#define _GNU_SOURCE

#include "client.h"
#include "common.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "stdio.h"
#include "time.h"
#include "sched.h"
#include "signal.h"

pthread_mutex_t data_buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

int messages_waiting;
int message_buff_size;
char **messages;
sigset_t zero_mask;

pthread_t sender_receiver_thread;
pthread_t io_thread;

int main(int argc, char *argv[]) {

    if(argc != EXPECTED_NUM_PARAMS+1) {

        printf("Usage: client <id> <type> <socket info>\n");
        printf("\t<id> - alphanumeric client identifier\n");
        printf("\t<type> - server type (LOCAL or REMOTE)\n");
        printf("\t<socket info> - if <type> is LOCAL, then <socket info> is path to a socket in file system, if REMOTE, than it is remote server IP addres followed by a port number\n");

        exit(1);
    }

    init_msg_buff();

    int ret_status;

    char *my_id = argv[1];
//    char *type = argv[2];
    char *char_ip = argv[3];
    char *char_port_num = argv[4];
    sender_receiver_thread = pthread_self();

    // prepare socket data
    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    ret_status = getaddrinfo(char_ip, char_port_num, &hints, &res);
    if(ret_status != 0) {
        error("getaddrinfo error");
    }

    sockfd = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);

    init_signals();

    // register this id in server
    send(sockfd, my_id, strlen(my_id), 0);

    // start io thread
    if((pthread_create(&(io_thread), NULL, stdin_read_fun, NULL)) != 0) {
        error("Thread creation error");
    }

    // main loop: wait for signal and send data, when available
    sender_receiver_fun(sockfd);

    exit(0);
}

void sender_receiver_fun(int sockfd) {

    int num_msgs_to_send;
    char **msgs_cpy;

    while(1 > 0) {

        // wait for signal
        sigsuspend(&zero_mask);
        if(messages_waiting == 0) {
            continue;
        } else {
            num_msgs_to_send = messages_waiting;
        }

        // enter critical section and copy waiting messages
        lock_data_mutex();
        {
            msgs_cpy = (char **) malloc(messages_waiting * sizeof(char *));
            if(msgs_cpy == NULL) {
                mem_error();
            }
            for(int i = 0; i < messages_waiting; ++i) {
                *(msgs_cpy+i) = get_copy(*(messages+i));
                memset(*(messages+1), 0, MSG_MAX_LEN);
            }
            messages_waiting = 0;
        }
        unlock_data_mutex();

        // send messages
        for(int i = 0; i < num_msgs_to_send; ++i) {
            char * msg_to_send = *(msgs_cpy+i);
            printf("Message to send: %s\n", msg_to_send);
            send(sockfd, msg_to_send, strlen(msg_to_send)+1, 0);
            free(msg_to_send);
        }
        free(msgs_cpy);
    }
}

void init_signals() {
    sigset_t usr_mask;
    if(sigemptyset(&usr_mask) < 0) {
        error("Sigemptyset error");
    }
    if(sigemptyset(&zero_mask) < 0) {
        error("Sigemptyset error");
    }
    if(sigaddset(&usr_mask, SIGUSR1) < 0) {
        error("Sigaddset error");
    }
    struct sigaction new_act_usr;
    new_act_usr.sa_handler = sig_handle;
    new_act_usr.sa_mask = usr_mask;
    if(sigaction(SIGUSR1, &new_act_usr, NULL) < 0) {
        error("Sigaction error for SIGUSR1");
    }
}

void init_msg_buff() {

    messages_waiting = 0;
    message_buff_size = 10;
    messages = (char **) malloc(message_buff_size* sizeof(char *));

    if(messages == NULL) {
        mem_error();
    }

    for(int i = 0; i < message_buff_size; ++i) {
        *(messages+i) = (char *) malloc(MSG_MAX_LEN+2);
        if(*(messages+i) == NULL) {
            mem_error();
        }
    }
}

void *stdin_read_fun(void *arg_ptr) {

    int buff_size = MSG_MAX_LEN+2;
//    int characters_read;
    char *msg_buf;
    int n;

    if((msg_buf = (char *) malloc(buff_size)) == NULL) {
        mem_error();
    }

    while(1) {

        n = buff_size;
        getline(&msg_buf, (size_t*)&n, stdin);

        lock_data_mutex();
        {
            // resize the buffer, if it's full
            if(messages_waiting == message_buff_size) {
                enlarge_msg_buff();
            }

            // append message at the end of the buffer
            strcpy(*(messages+messages_waiting), msg_buf);
            ++messages_waiting;
        }
        unlock_data_mutex();
        pthread_kill(sender_receiver_thread, SIGUSR1);
    }
    return NULL;
}

void enlarge_msg_buff() {

    message_buff_size *= 2;
    char **new_messages = (char **) malloc(message_buff_size* sizeof(char *));
    if(new_messages == NULL) {
                    mem_error();
                }
    for(int i = 0; i < message_buff_size; ++i) {
                    *(new_messages+i) = (char *) malloc(MSG_MAX_LEN+2);
                    if(*(new_messages+i) == NULL) {
                        mem_error();
                    }
                    strcpy(*(new_messages+i), *(messages+i));
                    free(*(messages+i));
                }
    free(messages);
    messages = new_messages;
}

void lock_data_mutex() {
    if(pthread_mutex_lock (&data_buffer_mutex) != 0) {
        error("Mutex locking error");
    }
}

void unlock_data_mutex() {
    if(pthread_mutex_unlock (&data_buffer_mutex) != 0) {
        error("Mutex unlocking error");
    }
}

void sig_handle(int signo) {
    // just ignore
}