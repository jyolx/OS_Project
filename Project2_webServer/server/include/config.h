#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int port;
    char address[15];
    int max_threads;
    char document_root[256];
} ServerConfig;

int load_config(const char *filename, ServerConfig *config);

#endif
