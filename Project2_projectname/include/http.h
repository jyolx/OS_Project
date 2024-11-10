#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <string.h>

typedef struct {
    char method[8];
    char path[128];
    char version[16];
    char auth[64];
} HttpRequest;

void parse_request(const char *buffer, HttpRequest *request);
void respond(int client_fd, HttpRequest *request);

#endif
