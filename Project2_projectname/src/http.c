#include "http.h"
#include "authentication.h"
#include <sys/socket.h>
#include <sys/stat.h>

#define DATA_DIR "data/" // Define the data directory

// Send an HTTP response to the client
void send_response(int client_socket, const char *status, const char *body)
{
    char response[BUFFER_SIZE];
    snprintf(response, BUFFER_SIZE,
             "HTTP/1.1 %s\r\n"
             "Content-Length: %ld\r\n"
             "Content-Type: text/plain\r\n"
             "\r\n%s", status, strlen(body), body);
    send(client_socket, response, strlen(response), 0);
}

// Function to check if a file exists in the data directory
int file_exists(const char *path)
{
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", path);
    struct stat buffer;
    return (stat(full_path, &buffer) == 0);
}

// Handle different HTTP methods
void handle_request(int client_socket, const char buffer[], const char client_ip[], int client_port)
{
    char method[10], path[1024];
    sscanf(buffer, "%s %s", method, path);

    // Check authorization if the path is secure
    const char *auth_header = strstr(buffer, "Authorization: ");
    if (strstr(path, "/secure") && !authenticate_request(auth_header ? auth_header + 15 : NULL))
    {
        send_response(client_socket, "401 Unauthorized", "Unauthorized");
        return;
    }

    // Construct full file path with data directory
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", path + 1);  // Skip leading '/'

    // Handle HTTP methods
    if (strcmp(method, "GET") == 0) {
        if (file_exists(path + 1)) {
            FILE *file = fopen(full_path, "r");
            if (file) {
                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                fseek(file, 0, SEEK_SET);
                char *file_content = malloc(file_size + 1);
                fread(file_content, 1, file_size, file);
                fclose(file);
                file_content[file_size] = '\0';

                send_response(client_socket, "200 OK", file_content);
                free(file_content);
            } else {
                send_response(client_socket, "500 Internal Server Error", "Unable to read file.");
            }
        } else {
            send_response(client_socket, "404 Not Found", "Resource not found.");
        }
    } else if (strcmp(method, "POST") == 0) {
        const char *data = strstr(buffer, "\r\n\r\n") + 4;
        if (data) {
            FILE *file = fopen(full_path, "w");
            if (file) {
                fwrite(data, 1, strlen(data), file);
                fclose(file);
                send_response(client_socket, "201 Created", "Resource created.");
            } else {
                send_response(client_socket, "500 Internal Server Error", "Unable to write to file.");
            }
        } else {
            send_response(client_socket, "400 Bad Request", "No data provided in POST request.");
        }
    } else if (strcmp(method, "PUT") == 0) {
        const char *data = strstr(buffer, "\r\n\r\n") + 4;
        if (data) {
            FILE *file = fopen(full_path, "w");
            if (file) {
                fwrite(data, 1, strlen(data), file);
                fclose(file);
                send_response(client_socket, "200 OK", "Resource updated.");
            } else {
                send_response(client_socket, "500 Internal Server Error", "Unable to write to file.");
            }
        } else {
            send_response(client_socket, "400 Bad Request", "No data provided in PUT request.");
        }
    } else if (strcmp(method, "DELETE") == 0) {
        if (file_exists(path + 1)) {
            if (remove(full_path) == 0) {
                send_response(client_socket, "200 OK", "Resource deleted.");
            } else {
                send_response(client_socket, "500 Internal Server Error", "Unable to delete file.");
            }
        } else {
            send_response(client_socket, "404 Not Found", "Resource not found.");
        }
    } else {
        send_response(client_socket, "400 Bad Request", "Invalid HTTP method.");
    }
}

/*

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
*/
