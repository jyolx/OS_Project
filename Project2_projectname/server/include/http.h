#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

void handle_request(int client_fd, const char buffer[], const char client_ip[], int client_port);

#endif