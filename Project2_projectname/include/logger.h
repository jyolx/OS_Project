#ifndef LOGGER_H
#define LOGGER_H

#include "http.h"

void log_request(HttpRequest *request, char client_ip[], int client_port);
void log_statement(char string[]);
void init_logger(const char *filename);
void close_logger();

#endif
