#include "http.h"
#include "authentication.h"
#include "logger.h"

#include <sys/socket.h>
#include <sys/stat.h>

#define DATA_DIR "data/" 

int file_exists(const char *path);
const char* get_content_type(const char *path);
void send_response(int client_socket, const char *status, const char *body, const char* content_type, size_t body_length);
void get_request(char log[], char full_path[], char path[], const char *client_ip, int client_port, int client_socket, const char *content_type);
void post_request(char log[], char full_path[], const char buffer[], const char *client_ip, int client_port, int client_socket);
void put_request(char log[], char full_path[], const char buffer[], const char *client_ip, int client_port, int client_socket);
void delete_request(char log[], char full_path[], char path[], const char *client_ip, int client_port, int client_socket);

void handle_request(int client_socket, const char buffer[], const char client_ip[], int client_port)
{
    char method[10], path[1024], log[1024];
    sscanf(buffer, "%s %s", method, path);

    // Check authorization if the path is secure
    const char *auth_header = strstr(buffer, "Authorization: ");

    if (strstr(path, "/secure"))
    {
        if(authenticate_request(auth_header ? auth_header + 15 : NULL) == 0)
        {
            bzero(log, sizeof(log));
            snprintf(log, sizeof(log), "Client %s:%d -> Invalid credentials...authorization failed", client_ip, client_port);
            log_statement(log);
            send_response(client_socket, "401 Unauthorized", "Unauthorized\n", "text/plain", strlen("Unauthorized\n"));
            return;
        }
        else if(authenticate_request(auth_header ? auth_header + 15 : NULL) == 1)
        {
            bzero(log, sizeof(log));
            snprintf(log, sizeof(log), "Client %s:%d -> Valid credentials...authorization success", client_ip, client_port);
            log_statement(log);
        }
        else if(authenticate_request(auth_header ? auth_header + 15 : NULL) == 2)
        {
            bzero(log, sizeof(log));
            snprintf(log, sizeof(log), "Client %s:%d -> Memory allocation... authorization failed", client_ip, client_port);
            log_statement(log);
            send_response(client_socket, "500 Internal Server Error", "Memory allocation failed\n", "text/plain", strlen("Memory allocation failed\n"));
            return;
        }
        else if(authenticate_request(auth_header ? auth_header + 15 : NULL) == 3)
        {
            bzero(log, sizeof(log));
            snprintf(log, sizeof(log), "Client %s:%d -> Unable to open users file...authentication failed", client_ip, client_port);
            log_statement(log);
            send_response(client_socket, "500 Internal Server Error", "Authorization cannot be right now\n", "text/plain", strlen("Authorization cannot be right now\n"));
            return;
        }
    }

    // Construct full file path with data directory
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", path + 1);

    const char *content_type = get_content_type(full_path);

    // Handle HTTP methods
    if (strcmp(method, "GET") == 0)
    {
        get_request(log, full_path, path, client_ip, client_port, client_socket, content_type);   
    }
    else if (strcmp(method, "POST") == 0)
    {
        post_request(log, full_path, buffer, client_ip, client_port, client_socket);
    }
    else if (strcmp(method, "PUT") == 0)
    {
        put_request(log, full_path, buffer, client_ip, client_port, client_socket);
    }
    else if (strcmp(method, "DELETE") == 0)
    {
        delete_request(log, full_path, path, client_ip, client_port, client_socket);
    }
    else
    {
        bzero(log, sizeof(log));
        snprintf(log, sizeof(log), "Client %s:%d -> Invalid HTTP method: %s", client_ip, client_port, method);
        log_statement(log);
        send_response(client_socket, "400 Bad Request", "Invalid HTTP method.\n", "text/plain", strlen("Invalid HTTP method.\n"));
    }
};

int file_exists(const char *path)
{
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", path);
    struct stat buffer;
    return (stat(full_path, &buffer) == 0);
};

const char* get_content_type(const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream"; // Default binary type

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".txt") == 0) return "text/plain";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
    if (strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".svg") == 0) return "image/svg+xml";
    if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
    if (strcmp(ext, ".wav") == 0) return "audio/wav";
    if (strcmp(ext, ".mp4") == 0) return "video/mp4";
    if (strcmp(ext, ".avi") == 0) return "video/x-msvideo";
    if (strcmp(ext, ".mpeg") == 0) return "video/mpeg";
    if (strcmp(ext, ".pdf") == 0) return "application/pdf";

    return "application/octet-stream"; // Default binary type
};


void send_response(int client_socket, const char *status, const char *body, const char* content_type, size_t body_length)
{
    char response[BUFFER_SIZE];

    int header_length = snprintf(response, BUFFER_SIZE,
             "HTTP/1.1 %s\r\n"
             "Content-Length: %ld\r\n"
             "Content-Type: %s\r\n"
             "\r\n", status, body_length, content_type);

    send(client_socket, response, header_length, 0);
    send(client_socket, body, body_length, 0);
};

void get_request(char log[], char full_path[], char path[], const char *client_ip, int client_port, int client_socket, const char *content_type)
{
    bzero(log, 1024);
    snprintf(log, 1024, "Client %s:%d -> GET : %s",client_ip, client_port, full_path);
    log_statement(log);
    if (file_exists(path + 1))
    {
        FILE *file = fopen(full_path, "rb");  // Open file in binary mode
        if (file)
        {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            char *file_content = malloc(file_size);
            fread(file_content, 1, file_size, file);
            fclose(file);
            bzero(log, 1024);
            snprintf(log, 1024, "Client %s:%d -> sending file %s", client_ip, client_port, full_path);
            log_statement(log);
            send_response(client_socket, "200 OK\n", file_content, content_type, file_size);
            free(file_content);
        }
        else
        {
            bzero(log, 1024);
            snprintf(log, 1024, "Client %s:%d -> Unable to read file: %s", client_ip, client_port, full_path);
            log_statement(log);
            send_response(client_socket, "500 Internal Server Error", "Unable to read file.\n", "text/plain", strlen("Unable to read file.\n"));
        }
    }
    else
    {
        bzero(log, 1024);
        snprintf(log, 1024, "Client %s:%d -> Resource not found: %s", client_ip, client_port, full_path);
        log_statement(log);
        send_response(client_socket, "404 Not Found", "Resource not found.\n", "text/plain", strlen("Resource not found.\n"));
    }
};

void post_request(char log[], char full_path[], const char buffer[],  const char *client_ip, int client_port, int client_socket)
{
    bzero(log, 1024);
    snprintf(log, 1024, "Client %s:%d -> POST : %s", client_ip, client_port, full_path);
    log_statement(log);
    const char *data = strstr(buffer, "\r\n\r\n") + 4;
    if (data)
    {
        FILE *file = fopen(full_path, "wb");  // Open file in binary mode
        if (file)
        {
            fwrite(data, 1, strlen(data), file);
            fclose(file);
            bzero(log, 1024);
            snprintf(log, 1024, "Client %s:%d -> Resource created: %s", client_ip, client_port, full_path);
            log_statement(log);
            send_response(client_socket, "201 Created", "Resource created.\n", "text/plain", strlen("Resource created.\n"));
        }
        else
        {
            bzero(log, 1024);
            snprintf(log, 1024, "Client %s:%d -> Unable to write to file: %s", client_ip, client_port, full_path);
            log_statement(log);
            send_response(client_socket, "500 Internal Server Error", "Unable to write to file.\n", "text/plain", strlen("Unable to write to file.\n"));
        }
    }
    else
    {
        bzero(log, 1024);
        snprintf(log, 1024, "Client %s:%d -> No data provided in POST request.", client_ip, client_port);
        log_statement(log);
        send_response(client_socket, "400 Bad Request", "No data provided in POST request.\n", "text/plain", strlen("No data provided in POST request.\n"));
    }
};

void put_request(char log[], char full_path[], const char buffer[],const char *client_ip, int client_port, int client_socket)
{
    bzero(log, 1024);
    snprintf(log, 1024, "Client %s:%d -> PUT : %s", client_ip, client_port, full_path);
    log_statement(log);
    const char *data = strstr(buffer, "\r\n\r\n") + 4;
    if (data)
    {
        FILE *file = fopen(full_path, "wb");  // Open file in binary mode
        if (file)
        {
            fwrite(data, 1, strlen(data), file);
            fclose(file);
            bzero(log, 1024);
            snprintf(log, 1024, "Client %s:%d -> Resource updated: %s", client_ip, client_port, full_path);
            log_statement(log);
            send_response(client_socket, "200 OK", "Resource updated.\n", "text/plain", strlen("Resource updated.\n"));
        }
        else
        {
            bzero(log, 1024);
            snprintf(log, 1024, "Client %s:%d -> Unable to write to file: %s", client_ip, client_port, full_path);
            log_statement(log);
            send_response(client_socket, "500 Internal Server Error", "Unable to write to file.\n", "text/plain", strlen("Unable to write to file.\n"));
        }
    }
    else
    {
        bzero(log, 1024);
        snprintf(log, 1024, "Client %s:%d -> No data provided in PUT request.", client_ip, client_port);
        log_statement(log);
        send_response(client_socket, "400 Bad Request", "No data provided in PUT request.\n", "text/plain", strlen("No data provided in PUT request.\n"));
    }
};

void delete_request(char log[], char full_path[], char path[], const char *client_ip, int client_port, int client_socket)
{
    if (file_exists(path + 1))
    {
        if (remove(full_path) == 0)
        {
            bzero(log, 1024);
            snprintf(log, 1024, "Client %s:%d -> Resource deleted: %s", client_ip, client_port, full_path);
            log_statement(log);
            send_response(client_socket, "200 OK", "Resource deleted.\n", "text/plain", strlen("Resource deleted.\n"));
        }
        else
        {
            bzero(log, 1024);
            snprintf(log, 1024, "Client %s:%d -> Unable to delete file: %s", client_ip, client_port, full_path);
            log_statement(log);
            send_response(client_socket, "500 Internal Server Error", "Unable to delete file.\n", "text/plain", strlen("Unable to delete file.\n"));
        }
    }
    else
    {
        bzero(log, 1024);
        snprintf(log, 1024, "Client %s:%d -> Resource not found: %s", client_ip, client_port, full_path);
        log_statement(log);
        send_response(client_socket, "404 Not Found", "Resource not found.\n", "text/plain", strlen("Resource not found.\n"));
    }
};
