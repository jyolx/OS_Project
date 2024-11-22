#include "server.h"
#include "config.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    init_logger("logs/server.log");

    ServerConfig config;
    if (load_config("config/server.conf", &config) != 0)
    {
        fprintf(stderr, "Failed to load configuration file.\n");
        return 1;
    }
    log_statement("Configuration file loaded successfully.");

    start_server(&config);

    return 0;
};
