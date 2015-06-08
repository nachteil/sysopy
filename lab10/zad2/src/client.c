#define _GNU_SOURCE

#include "client.h"
#include "common.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include "sys/select.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "string.h"
#include <sys/un.h>
#include "stdlib.h"
#include "errno.h"
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

int sck;

void usage();

int main(int argc, char *argv[]) {

    if(argc < 4) {
        usage();
        exit(1);
    }

    init_msg_buff();

    char *my_id = argv[1];
    char *type = argv[2];

    sender_receiver_thread = pthread_self();

    if(strcmp(type, "LOCAL") && strcmp(type, "REMOTE")) {
        usage();
        printf("Unrecognized server type\n");
        exit(1);
    }

    if(!strcmp(type, "LOCAL")) {
        if(access(argv[3], F_OK) == -1) {
            error("Socket file does not exist");
        }
        sck = create_unix_socket(argv);
    } else {
        sck = create_internet_socket(argv);
    }

    init_signals();

    // start io thread
    if((pthread_create(&(io_thread), NULL, stdin_read_fun, (void *)my_id)) != 0) {
        error("Thread creation error");
    }

    // main loop: wait for signal and send data, when available
    sender_receiver_fun(sck);

    exit(0);
}

void usage() {
    printf("Usage: client <id> <type> <socket info>\n");
    printf("\t<id> - alphanumeric client identifier\n");
    printf("\t<type> - server type (LOCAL or REMOTE)\n");
    printf("\t<socket info> - if <type> is LOCAL, then <socket info> is path to a socket in file system, if REMOTE, than it is remote server IP addres followed by a port number\n");
}

void sender_receiver_fun(int sockfd) {

    fd_set orig;
    fd_set read_fd;

    FD_ZERO(&orig);
    FD_ZERO(&read_fd);

    FD_SET(sockfd, &orig);

    char buf[350];
    int status;

    while(1 > 0) {

        read_fd = orig;
        status = select(sockfd+1, &read_fd, NULL, NULL, NULL);
        // if we got interrupted by signal, it's OK - just signal handler sent message
        if (status == -1 && errno != EINTR) {
            error("select() error");
            exit(-1);
        }

        if(status != -1 && FD_ISSET(sockfd, &read_fd)) {
            memset(buf, 0, 350);
            int nbytes = recv(sockfd, buf, 350, 0);
            if(nbytes <= 0) {
                printf("Server closed connection\n");
                exit(0);
            }
            printf("%s\n", buf);
        }
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

    char *my_id = (char *)arg_ptr;
    int buff_size = MSG_MAX_LEN+2;
    char *msg_buf;
    char *msg_with_header;
    int n;
    int id_len = strlen(my_id);

    if((msg_buf = (char *) malloc(buff_size)) == NULL) {
        mem_error();
    }
    if((msg_with_header = (char *) malloc(buff_size + id_len+1)) == NULL) {
        mem_error();
    }

    while(1) {

        n = buff_size;
        getline(&msg_buf, (size_t*)&n, stdin);
        printf("\n");
        strcpy(msg_with_header, my_id);
        msg_with_header[id_len] = ':';
        msg_with_header[id_len+1] = '\n';
        strcpy(&(msg_with_header[id_len+2]), msg_buf);

        lock_data_mutex();
        {
            // resize the buffer, if it's full
            if(messages_waiting == message_buff_size) {
                enlarge_msg_buff();
            }

            // append message at the end of the buffer
            strcpy(*(messages+messages_waiting), msg_with_header);
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

    int num_msgs_to_send;
    char **msgs_cpy;

    if(messages_waiting == 0) {
        return;
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
        if(-1 == send(sck, msg_to_send, strlen(msg_to_send)+1, 0)) {
            error("Connection error during send()");
        }
        free(msg_to_send);
    }
    free(msgs_cpy);
    init_signals();
}

int create_unix_socket(char *argv[]) {

    int socket_fd, len;
    struct sockaddr_un remote;
    char *unix_socket_path = argv[3];

    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, unix_socket_path);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(socket_fd, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }
    return socket_fd;
}

int create_internet_socket(char *argv[]) {

    struct addrinfo hints, *res;

    char *char_ip = argv[3];
    char *char_port_num = argv[4];

    int ret_status, sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    ret_status = getaddrinfo(char_ip, char_port_num, &hints, &res);
    if(ret_status != 0) {
        error("getaddrinfo error");
    }

    if((sockfd = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol)) == -1) {
        error("socket()");
    }
    if((connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
        error("connect()");
    }
    sck = sockfd;
    printf("Connected to server\n");

    return sck;
}