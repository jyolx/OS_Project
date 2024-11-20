#include "http.h"
#include "authentication.h"
#include <sys/socket.h>
#include <sys/stat.h>

#define DATA_DIR "data/" // Define the data directory

// Function to determine the content type based on the file extension
const char* get_content_type(const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream"; // Default binary type

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
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

    return "application/octet-stream"; // Default binary type
}

// Send an HTTP response to the client
void send_response(int client_socket, const char *status, const char *body,const char* content_type)
{
    char response[BUFFER_SIZE];

    snprintf(response, BUFFER_SIZE,
             "HTTP/1.1 %s\r\n"
             "Content-Length: %ld\r\n"
             "Content-Type: %s\r\n"
             "\r\n%s", status, strlen(body),content_type, body);

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
        send_response(client_socket, "401 Unauthorized", "Unauthorized", "text/plain");
        return;
    }

    // Construct full file path with data directory
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", path + 1);  // Skip leading '/'

    // Determine the content type
    const char *content_type = get_content_type(full_path);

    // Handle HTTP methods
    if (strcmp(method, "GET") == 0)
    {
        if (file_exists(path + 1))
        {
            FILE *file = fopen(full_path, "r");
            if (file)
            {
                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                fseek(file, 0, SEEK_SET);
                char *file_content = malloc(file_size + 1);
                fread(file_content, 1, file_size, file);
                fclose(file);
                file_content[file_size] = '\0';

                send_response(client_socket, "200 OK", file_content, content_type);
                free(file_content);
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "Unable to read file.", "text/plain");
            }
        }
        else
        {
            send_response(client_socket, "404 Not Found", "Resource not found.", "text/plain");
        }
    }
    else if (strcmp(method, "POST") == 0)
    {
        const char *data = strstr(buffer, "\r\n\r\n") + 4;
        if (data)
        {
            FILE *file = fopen(full_path, "w");
            if (file)
            {
                fwrite(data, 1, strlen(data), file);
                fclose(file);
                send_response(client_socket, "201 Created", "Resource created.", "text/plain");
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "Unable to write to file.", "text/plain");
            }
        }
        else
        {
            send_response(client_socket, "400 Bad Request", "No data provided in POST request.", "text/plain");
        }
    }
    else if (strcmp(method, "PUT") == 0)
    {
        const char *data = strstr(buffer, "\r\n\r\n") + 4;
        if (data)
        {
            FILE *file = fopen(full_path, "w");
            if (file)
            {
                fwrite(data, 1, strlen(data), file);
                fclose(file);
                send_response(client_socket, "200 OK", "Resource updated.", "text/plain");
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "Unable to write to file.", "text/plain");
            }
        }
        else
        {
            send_response(client_socket, "400 Bad Request", "No data provided in PUT request.", "text/plain");
        }
    }
    else if (strcmp(method, "DELETE") == 0)
    {
        if (file_exists(path + 1))
        {
            if (remove(full_path) == 0)
            {
                send_response(client_socket, "200 OK", "Resource deleted.", "text/plain");
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "Unable to delete file.", "text/plain");
            }
        }
        else
        {
            send_response(client_socket, "404 Not Found", "Resource not found.", "text/plain");
        }
    }
    else
    {
        send_response(client_socket, "400 Bad Request", "Invalid HTTP method.", "text/plain");
    }
}

