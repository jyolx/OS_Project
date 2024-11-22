#include "http.h"
#include "authentication.h"
#include <sys/socket.h>
#include <sys/stat.h>

#define DATA_DIR "data/" // Define the data directory

/**
 * @brief Function to determine the content type based on the file extension
 * @param path The file path
 * @return The content type
 */
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
}

/**
 * @brief Function to send an HTTP response
 * @param client_socket The client socket
 * @param status The HTTP status code
 * @param body The response body
 * @param content_type The content type
 * @param body_length The length of the response body
 */
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
}

/**
 * @brief Function to check if a file exists in the data directory
 * @param path The file path
 * @return 1 if the file exists, 0 otherwise
 */
int file_exists(const char *path)
{
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", path);
    struct stat buffer;
    return (stat(full_path, &buffer) == 0);
}

/**
 * @brief Handle different HTTP methods
 * @param client_socket The client socket
 * @param buffer The HTTP request buffer
 * @param client_ip The client IP address
 * @param client_port The client port
*/
void handle_request(int client_socket, const char buffer[], const char client_ip[], int client_port)
{
    char method[10], path[1024], log[1024];
    sscanf(buffer, "%s %s", method, path);

    // Check authorization if the path is secure
    const char *auth_header = strstr(buffer, "Authorization: ");
    if (strstr(path, "/secure") && !authenticate_request(auth_header ? auth_header + 15 : NULL))
    {
        bzero(log, sizeof(log));
        snprintf(log, sizeof(log), "Unauthorized access attempt from %s:%d", client_ip, client_port);
        log_statement(log);
        send_response(client_socket, "401 Unauthorized", "Unauthorized", "text/plain", strlen("Unauthorized"));
        return;
    }

    // Construct full file path with data directory
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", path + 1);

    const char *content_type = get_content_type(full_path);

    // Handle HTTP methods
    if (strcmp(method, "GET") == 0)
    {
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

                send_response(client_socket, "200 OK", file_content, content_type, file_size);
                free(file_content);
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "Unable to read file.", "text/plain", strlen("Unable to read file."));
            }
        }
        else
        {
            send_response(client_socket, "404 Not Found", "Resource not found.", "text/plain", strlen("Resource not found."));
        }
    }
    else if (strcmp(method, "POST") == 0)
    {
        const char *data = strstr(buffer, "\r\n\r\n") + 4;
        if (data)
        {
            FILE *file = fopen(full_path, "wb");  // Open file in binary mode
            if (file)
            {
                fwrite(data, 1, strlen(data), file);
                fclose(file);
                send_response(client_socket, "201 Created", "Resource created.", "text/plain", strlen("Resource created."));
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "Unable to write to file.", "text/plain", strlen("Unable to write to file."));
            }
        }
        else
        {
            send_response(client_socket, "400 Bad Request", "No data provided in POST request.", "text/plain", strlen("No data provided in POST request."));
        }
    }
    else if (strcmp(method, "PUT") == 0)
    {
        const char *data = strstr(buffer, "\r\n\r\n") + 4;
        if (data)
        {
            FILE *file = fopen(full_path, "wb");  // Open file in binary mode
            if (file)
            {
                fwrite(data, 1, strlen(data), file);
                fclose(file);
                send_response(client_socket, "200 OK", "Resource updated.", "text/plain", strlen("Resource updated."));
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "Unable to write to file.", "text/plain", strlen("Unable to write to file."));
            }
        }
        else
        {
            send_response(client_socket, "400 Bad Request", "No data provided in PUT request.", "text/plain", strlen("No data provided in PUT request."));
        }
    }
    else if (strcmp(method, "DELETE") == 0)
    {
        if (file_exists(path + 1))
        {
            if (remove(full_path) == 0)
            {
                send_response(client_socket, "200 OK", "Resource deleted.", "text/plain", strlen("Resource deleted."));
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "Unable to delete file.", "text/plain", strlen("Unable to delete file."));
            }
        }
        else
        {
            send_response(client_socket, "404 Not Found", "Resource not found.", "text/plain", strlen("Resource not found."));
        }
    }
    else
    {
        send_response(client_socket, "400 Bad Request", "Invalid HTTP method.", "text/plain", strlen("Invalid HTTP method."));
    }
}
