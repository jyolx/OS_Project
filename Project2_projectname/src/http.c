#include "http.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <stdlib.h>

void parse_request(const char *buffer, HttpRequest *request) {
    // Parse the request line
    sscanf(buffer, "%s %s %s", request->method, request->path, request->version);

    // Extract the body if the method is POST or PUT
    if (strcmp(request->method, "POST") == 0 || strcmp(request->method, "PUT") == 0) {
        const char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
            strncpy(request->body, body + 4, sizeof(request->body) - 1);
        }
    }
}

void respond(int client_fd, HttpRequest *request) {
    if (strcmp(request->method, "GET") == 0) {
        handle_get_request(client_fd, request);
    } else if (strcmp(request->method, "POST") == 0) {
        handle_post_request(client_fd, request);
    } else if (strcmp(request->method, "PUT") == 0) {
        handle_put_request(client_fd, request);
    } else if (strcmp(request->method, "DELETE") == 0) {
        handle_delete_request(client_fd, request);
    } else {
        handle_unsupported_method(client_fd);
    }
    
}

void handle_get_request(int client_fd, HttpRequest *request) {
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "www%s", request->path);
    if (strcmp(request->path, "/") == 0) {
        strcpy(file_path, "www/index.html");
    }

    int file_fd = open(file_path, O_RDONLY);
    if (file_fd < 0) {
        send_404_response(client_fd);
    } else {
        struct stat file_stat;
        fstat(file_fd, &file_stat);
        char response[1024];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", file_stat.st_size);
        send(client_fd, response, strlen(response), 0);
        sendfile(client_fd, file_fd, NULL, file_stat.st_size);
        close(file_fd);

    }
}

void handle_post_request(int client_fd, HttpRequest *request) {
    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPOST request received";
    send(client_fd, response, sizeof(response) - 1, 0);
}

void handle_put_request(int client_fd, HttpRequest *request) {
    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPUT request received";
    send(client_fd, response, sizeof(response) - 1, 0);
}

void handle_delete_request(int client_fd, HttpRequest *request) {
    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nDELETE request received";
    send(client_fd, response, sizeof(response) - 1, 0);
}

void handle_unsupported_method(int client_fd) {
    char not_implemented[] = "HTTP/1.1 501 Not Implemented\r\n";
    send(client_fd, not_implemented, sizeof(not_implemented) - 1, 0);
}

void send_404_response(int client_fd) {
    char not_found[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
    send(client_fd, not_found, sizeof(not_found) - 1, 0);
}

void open_url_in_browser(const char *path) {
    char url[256];
    snprintf(url, sizeof(url), "http://localhost:8080%s", path);

    #if defined(_WIN32) || defined(_WIN64)
        char command[512];
        snprintf(command, sizeof(command), "start %s", url);
        system(command);
    #elif defined(__APPLE__) || defined(__MACH__)
        char command[512];
        snprintf(command, sizeof(command), "open %s", url);
        system(command);
    #elif defined(__linux__)
        char command[512];
        snprintf(command, sizeof(command), "xdg-open %s", url);
        system(command);
    #else
        fprintf(stderr, "Unsupported OS\n");
    #endif
}