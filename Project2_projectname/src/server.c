#include "server.h"
#include "http.h"
#include "authentication.h"
#include "logger.h"

void *handle_client(void *arg);

void start_server(ServerConfig *config) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config->port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, (void *)&client_fd);
        pthread_detach(thread);
    }

    close(server_fd);
}

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    char buffer[1024];
    recv(client_fd, buffer, sizeof(buffer), 0);
    
    HttpRequest request;
    parse_request(buffer, &request);

    if (authenticate_request(&request)) {
        respond(client_fd, &request);
    } else {
        send(client_fd, "HTTP/1.1 401 Unauthorized\r\n", 26, 0);
    }

    log_request(&request);
    close(client_fd);
    return NULL;
}
