#include "logger.h"
#include <stdio.h>
#include <time.h>

FILE *log_file;

void log_statement(char string[])
{
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0'; // Remove newline character from time string
    fprintf(log_file, "[%s] - %s\n", time_str, string);
    fflush(log_file);
    printf("[%s] - %s\n", time_str, string);
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
