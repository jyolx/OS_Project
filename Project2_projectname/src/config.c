#include "config.h"
#include <stdio.h>
#include <string.h>

int load_config(const char *filename, ServerConfig *config) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "port=%d", &config->port) == 1) continue;
        if (sscanf(line, "document_root=%s", config->document_root) == 1) continue;
    }
    fclose(file);
    return 0;
}
