#include "logger.h"
#include <stdio.h>
#include <time.h>

FILE *log_file;

/*
void log_request(HttpRequest *request, char client_ip[], int client_port)
{
    time_t now = time(NULL);
    fprintf(log_file, "[%s] - %s %s %s Client -> %s:%d\n", ctime(&now), request->method, request->path, request->version, client_ip, client_port);
    fflush(log_file);
    printf("[%s] - %s %s %s Client -> %s:%d\n", ctime(&now), request->method, request->path, request->version, client_ip, client_port);
};
*/

void log_statement(char string[])
{
    time_t now = time(NULL);
    fprintf(log_file, "[%s] - %s\n", ctime(&now), string);
    fflush(log_file);
    printf("[%s] - %s\n", ctime(&now), string);
};

void init_logger(const char *filename)
{
    log_file = fopen(filename, "w");
    log_statement("Logger initialized");
};

void close_logger()
{
    if (log_file)
        fclose(log_file);
};
