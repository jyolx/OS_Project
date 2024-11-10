#include "server.h"
#include "config.h"
#include "logger.h"

int main(int argc, char *argv[]) {
    // Load configuration
    ServerConfig config;
    if (load_config("config/server.conf", &config) != 0) {
        fprintf(stderr, "Failed to load configuration.\n");
        return 1;
    }

    // Initialize logger
    init_logger("logs/server.log");

    // Start server
    start_server(&config);

    return 0;
}
