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

time_t in_ping_time[MAX_CLIENTS];
time_t un_ping_time[MAX_CLIENTS];
struct sockaddr_in in_clients[MAX_CLIENTS];
struct sockaddr_un un_clients[MAX_CLIENTS];

int main(int argc, char *argv[]) {

    if (argc != DESIRED_NUM_PARAMS + 1) {

        printf("Usage: server <port_no> <path>\n");
        printf("\t<port_no> - port number to listen on\n");
        printf("\t<path> - path to Unix domain socket file\n");

        exit(1);
    }

    for(int i = 0; i < MAX_CLIENTS; ++i) {
        in_ping_time[i] = -1;
        un_ping_time[i] = -1;
    }

    char *char_portnum = argv[1];
    char *unix_socket_path = argv[2];

    // create, initialize and listen on inet socket
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, char_portnum, &hints, &res);

    inet_listening_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(inet_listening_socket, res->ai_addr, res->ai_addrlen);
    listen(inet_listening_socket, MAX_WAITING_CONNECTIONS);

    // create, initialize and listen on unix socket
    struct sockaddr_un local;

    unix_listening_socket = socket(AF_UNIX, SOCK_DGRAM, 0);

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, unix_socket_path);
    unlink(local.sun_path);
//    int len = strlen(local.sun_path) + sizeof(local.sun_family);
    bind(unix_listening_socket, (struct sockaddr *)&local, sizeof (struct sockaddr));

    listen(unix_listening_socket, MAX_WAITING_CONNECTIONS);

    //

    fd_set orig;
    fd_set read_fd;

    FD_ZERO(&orig);
    FD_ZERO(&read_fd);

    FD_SET(inet_listening_socket, &orig);
    FD_SET(unix_listening_socket, &orig);

    char buf[350];
    int maxfd = inet_listening_socket > unix_listening_socket ? inet_listening_socket : unix_listening_socket;

    struct sockaddr_in their_addr_in;
    struct sockaddr_un their_addr_un;

    int numbytes;

    while(1 > 0) {

        int r;
        read_fd = orig;
        if ((r = select(maxfd+1, &read_fd, NULL, NULL, NULL)) == -1) {
            error("select() error");
            exit(-1);
        }

        if(FD_ISSET(inet_listening_socket, &read_fd)) {

            memset(buf, 0, 350);
            int addr_len = sizeof their_addr_in;
            if ((numbytes = recvfrom(inet_listening_socket, buf, 350 , 0,
                                     (struct sockaddr *)&their_addr_in, (socklen_t*)(&addr_len))) == -1) {
                error("recvfrom");
            }

            int sender_id = -1;
            for(int i = 0; i < MAX_CLIENTS; ++i) {
                if(in_ping_time[i] != -1) {
                    if(in_clients[i].sin_port == their_addr_in.sin_port &&
                            in_clients[i].sin_addr.s_addr == their_addr_in.sin_addr.s_addr) {
                        sender_id = i;
                        in_ping_time[sender_id] = time(NULL);
                        break;
                    }
                }
            }

            if(sender_id == -1) {

                int empty_slot = -1;
                for(int j = 0; j < MAX_CLIENTS; ++j) {
                    if(in_ping_time[j] == -1) {
                        empty_slot = j;
                        break;
                    }
                }
                if(empty_slot == -1) {
                    printf("Cannot accept more clients\n");
                } else {
                    in_ping_time[empty_slot] = time(NULL);
                    in_clients[empty_slot] = their_addr_in;
                    sender_id = empty_slot;
                }
            }

            for(int i = 0; i < MAX_CLIENTS; ++i) {
                if(strstr(buf, "--register--") != NULL) {
                    break;
                }
                if(sender_id != i) {
                    if(in_ping_time[i] != -1 && time(NULL) - in_ping_time[i] > 10) {
                        in_ping_time[i] = -1;
                    } else if(in_ping_time[i] != -1){
                        if(-1 == sendto(inet_listening_socket, buf, strlen(buf)+1, 0, (struct sockaddr *)&(in_clients[i]), sizeof(struct sockaddr_in))) {
                            error("Connection error during send()");
                        }
                    }
                }
                if(un_ping_time[i] != -1 && time(NULL) - un_ping_time[i] > 10) {
                    un_ping_time[i] = -1;
                } else if(un_ping_time[i] != -1) {
                    if(-1 == sendto(unix_listening_socket, buf, strlen(buf)+1, 0, (struct sockaddr *)&(un_clients[i]), sizeof(struct sockaddr_un))) {
                        error("Connection error during send()");
                    }
                }
            }
        } else {
            //
            memset(buf, 0, 350);
            int addr_len = sizeof their_addr_un;
            if ((numbytes = recvfrom(unix_listening_socket, buf, 350 , 0,
                                     (struct sockaddr *)&their_addr_un, (socklen_t*)(&addr_len))) == -1) {
                error("recvfrom");
            }

            int sender_id = -1;
            for(int i = 0; i < MAX_CLIENTS; ++i) {
                if(un_ping_time[i] != -1) {
                    if(!strcmp(un_clients[i].sun_path, their_addr_un.sun_path)) {
                        sender_id = i;
                        un_ping_time[sender_id] = time(NULL);
                        break;
                    }
                }
            }

            if(sender_id == -1) {

                int empty_slot = -1;
                for(int j = 0; j < MAX_CLIENTS; ++j) {
                    if(un_ping_time[j] == -1) {
                        empty_slot = j;
                        break;
                    }
                }
                if(empty_slot == -1) {
                    printf("Cannot accept more clients\n");
                } else {
                    un_ping_time[empty_slot] = time(NULL);
                    un_clients[empty_slot] = their_addr_un;
                    sender_id = empty_slot;
                }
            }

            for(int i = 0; i < MAX_CLIENTS; ++i) {
                if(strstr(buf, "--register--") != NULL) {
                    break;
                }
                if(sender_id != -1 && sender_id != i) {
                    if(un_ping_time[i] != -1 && time(NULL) - un_ping_time[i] > 10) {
                        un_ping_time[i] = -1;
                    } else if(un_ping_time[i] != -1){
                        if(-1 == sendto(unix_listening_socket, buf, strlen(buf)+1, 0, (struct sockaddr *)&(un_clients[i]), sizeof(struct sockaddr_un))) {
                            error("Connection error during send()");
                        }
                    }
                }
                if(in_ping_time[i] != -1 && time(NULL) - in_ping_time[i] > 10) {
                    in_ping_time[i] = -1;
                } else if (in_ping_time[i] != -1){
                    if(-1 == sendto(inet_listening_socket, buf, strlen(buf)+1, 0, (struct sockaddr *)&(in_clients[i]), sizeof(struct sockaddr_in))) {
                        error("Connection error during send()");
                    }
                }
            }
        }

    }

    exit(0);
}
