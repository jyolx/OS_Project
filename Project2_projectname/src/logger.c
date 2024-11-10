#include "logger.h"
#include <stdio.h>
#include <time.h>

FILE *log_file;

void init_logger(const char *filename) {
    log_file = fopen(filename, "a");
}

void log_request(HttpRequest *request) {
    time_t now = time(NULL);
    fprintf(log_file, "%s - %s %s %s\n", ctime(&now), request->method, request->path, request->version);
    fflush(log_file);
}

void close_logger() {
    if (log_file) fclose(log_file);
}
