#ifndef LOGGER_H
#define LOGGER_H

#include "http.h"

void init_logger(const char *filename);
void log_request(HttpRequest *request);
void close_logger();

#endif
