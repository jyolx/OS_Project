#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char method[8];
    char path[128];
    char version[16];
    char auth[64];
    char client_ip[16];
    char body[1024]; // Add body to handle POST/PUT data
} HttpRequest;

void parse_request(const char *buffer, HttpRequest *request);
void respond(int client_fd, HttpRequest *request);
void handle_get_request(int client_fd, HttpRequest *request);
void handle_post_request(int client_fd, HttpRequest *request);
void handle_put_request(int client_fd, HttpRequest *request);
void handle_delete_request(int client_fd, HttpRequest *request);
void handle_unsupported_method(int client_fd);
void send_404_response(int client_fd);
void open_url_in_browser(const char *path);

#endif