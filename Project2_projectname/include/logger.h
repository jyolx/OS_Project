#ifndef LOGGER_H
#define LOGGER_H

#include "http.h"

void log_request(HttpRequest *request);
void log_statement(char string[]);
void init_logger(const char *filename);
void close_logger();

#endif
