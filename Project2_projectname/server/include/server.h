#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include<sys/socket.h>

#include "config.h"

typedef struct Client_details {
    int client_fd;
    char client_ip[INET_ADDRSTRLEN];
    int client_port;
}Client_details;

void start_server(ServerConfig *config);

#endif
