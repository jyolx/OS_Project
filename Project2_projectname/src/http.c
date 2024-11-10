#include "http.h"
#include <sys/socket.h>

void parse_request(const char *buffer, HttpRequest *request) {
    sscanf(buffer, "%s %s %s", request->method, request->path, request->version);
}

void respond(int client_fd, HttpRequest *request) {
    if (strcmp(request->method, "GET") == 0 && strcmp(request->path, "/") == 0) {
        char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
        send(client_fd, response, sizeof(response) - 1, 0);
    } else {
        char not_found[] = "HTTP/1.1 404 Not Found\r\n";
        send(client_fd, not_found, sizeof(not_found) - 1, 0);
    }
}
