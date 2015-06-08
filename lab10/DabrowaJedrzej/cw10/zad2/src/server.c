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
#include <sys/un.h>
#include "unistd.h"
#include "time.h"
#include "sched.h"

int inet_listening_socket;
int unix_listening_socket;
int sockets[MAX_CLIENTS];

int main(int argc, char *argv[]) {

    if (argc != DESIRED_NUM_PARAMS + 1) {

        printf("Usage: server <port_no> <path>\n");
        printf("\t<port_no> - port number to listen on\n");
        printf("\t<path> - path to Unix domain socket file\n");

        exit(1);
    }

    for(int i = 0; i < MAX_CLIENTS; ++i) {
        sockets[i] = -1;
    }

    char *char_portnum = argv[1];
    char *unix_socket_path = argv[2];

    // create, initialize and listen on inet socket
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, char_portnum, &hints, &res);

    inet_listening_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    sockets[0] = inet_listening_socket;
    bind(inet_listening_socket, res->ai_addr, res->ai_addrlen);
    listen(inet_listening_socket, MAX_WAITING_CONNECTIONS);

    // create, initialize and listen on unix socket
    struct sockaddr_un local;
    int len;

    unix_listening_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, unix_socket_path);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    bind(unix_listening_socket, (struct sockaddr *)&local, len);

    listen(unix_listening_socket, MAX_WAITING_CONNECTIONS);
    sockets[1] = unix_listening_socket;

    //

    fd_set orig;
    fd_set read_fd;

    FD_ZERO(&orig);
    FD_ZERO(&read_fd);

    FD_SET(inet_listening_socket, &orig);
    FD_SET(unix_listening_socket, &orig);

    char buf[350];
    int maxfd = inet_listening_socket > unix_listening_socket ? inet_listening_socket : unix_listening_socket;
    int newfd;

    while(1 > 0) {

        int r;
        read_fd = orig;
        if ((r = select(maxfd+1, &read_fd, NULL, NULL, NULL)) == -1) {
            error("select() error");
            exit(-1);
        }

        for(int i = 0; i < MAX_CLIENTS; ++i) {

            if(sockets[i] != -1 && FD_ISSET(sockets[i], &read_fd)) {

                // accept new connection
                if(sockets[i] == inet_listening_socket || sockets[i] == unix_listening_socket) {
                    newfd = accept(sockets[i], NULL, NULL);
                    if(newfd == -1) {
                        error("accept() error");
                    }
                    int j;
                    for(j = 0; j < MAX_CLIENTS; ++j) {
                        if(sockets[j] == -1) {
                            sockets[j] = newfd;
                            break;
                        }
                    }
                    if(j == MAX_CLIENTS) {
                        printf("Cannot accept more clients\n");
                    } else {
                        maxfd = newfd > maxfd ? newfd : maxfd;
                        FD_SET(newfd, &orig);
                        printf("Got new client\n");
                    }

                } else {
                    int nbytes;
                    memset(buf, 0, 350);
                    nbytes = recv(sockets[i], buf, 350, 0);

                    if(nbytes <= 0) {
                        if (nbytes == 0) {
                            printf("%d closed connection ;(\n", i);
                        } else {
                            perror("revc()");
                        }
                        sockets[i] = -1;
                        close(sockets[i]);
                        FD_CLR(sockets[i], &orig);
                        maxfd = find_max();
                    } else {
                        for(int j = 0; j < MAX_CLIENTS; ++j) {
                            if(j != i && sockets[j] != -1 && sockets[j] != inet_listening_socket) {
                                send(sockets[j], buf, strlen(buf)+1, 0);
                                printf("Sending from %d to %d \n", i, j);
                            }
                        }
                    }
                }

            }
        }

    }

    exit(0);
}

int find_max() {

    int max = inet_listening_socket;
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        max = (sockets[i] != -1 && sockets[i] > max) ? sockets[i] : max;
    }
    return max;
}