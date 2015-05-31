#define _GNU_SOURCE

#include "server.h"
#include "stdlib.h"
#include "stdio.h"
#include "common.h"
#include "client.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include "string.h"
#include "unistd.h"
#include "time.h"
#include "sched.h"

int main(int argc, char *argv[]) {

    if (argc != DESIRED_NUM_PARAMS + 1) {

        printf("Usage: server <port_no> <path>\n");
        printf("\t<port_no> - port number to listen on\n");
        printf("\t<path> - path to Unix domain socket file\n");

        exit(1);
    }

    char msg_buf[MSG_MAX_LEN];

    char *char_portnum = argv[1];
//    char *unix_socket_path = argv[2];

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, char_portnum, &hints, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, MAX_WAITING_CONNECTIONS);

    addr_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);

    while (1) {
        recv(new_fd, msg_buf, MSG_MAX_LEN, 0);
        printf("Got message: %s\n", msg_buf);
    }
    exit(0);
}