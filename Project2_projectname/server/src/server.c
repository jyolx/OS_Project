#include "server.h"
#include "http.h"
#include "logger.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>

typedef struct {
    Client_details *queue;
    int front;
    int rear;
    int size;
    sem_t mutex;
    sem_t items;
    sem_t spaces;
}ClientQueue;

ClientQueue client_queue;

void init_queue(ClientQueue *q, int size)
{
    q->queue = (Client_details *)malloc(size * sizeof(Client_details));
    q->front = 0;
    q->rear = 0;
    q->size = size;
    sem_init(&q->mutex, 0, 1);
    sem_init(&q->items, 0, 0);
    sem_init(&q->spaces, 0, size);
    log_statement("Queue has been initialized.");
};

void enqueue(ClientQueue *q, int client_fd, char client_ip[], int client_port)
{
    sem_wait(&q->spaces);
    sem_wait(&q->mutex);

    q->queue[q->rear].client_fd = client_fd;
    strcpy(q->queue[q->rear].client_ip,client_ip);
    q->queue[q->rear].client_port = client_port;
    q->rear = (q->rear + 1) % q->size;

    char log_message[64];
    sprintf(log_message, "Client %s:%d has been added to the queue.", client_ip,client_port);
    log_statement(log_message);

    sem_post(&q->mutex);
    sem_post(&q->items);
};

Client_details* dequeue(ClientQueue *q)
{
    sem_wait(&q->items);
    sem_wait(&q->mutex);

    Client_details *client = (Client_details *)malloc(sizeof(Client_details));
    client->client_fd = q->queue[q->front].client_fd;
    strcpy(client->client_ip, q->queue[q->front].client_ip);
    client->client_port = q->queue[q->front].client_port;

    q->front = (q->front + 1) % q->size;

    char log_message[64];
    sprintf(log_message, "Client %s:%d has been removed from the queue.", client->client_ip, client->client_port);
    log_statement(log_message);

    sem_post(&q->mutex);
    sem_post(&q->spaces);
    return client;
};

void destroy_queue(ClientQueue *q)
{
    free(q->queue);
    sem_destroy(&q->mutex);
    sem_destroy(&q->items);
    sem_destroy(&q->spaces);
    log_statement("Queue has been destroyed.");
};

void handle_client(Client_details* client)
{
    char buffer[BUFFER_SIZE];
    recv(client->client_fd, buffer, sizeof(buffer), 0);

    handle_request(client->client_fd, buffer, client->client_ip, client->client_port);

    char log_message[128];
    sprintf(log_message, "Request from %s:%d has been handled. Closing client socket...", client->client_ip, client->client_port);    
    close(client->client_fd);
    free(client);
    client = NULL;
};

void *worker_thread(void *arg)
{
    while (1)
    {
        Client_details* client = dequeue(&client_queue);
        handle_client(client);
    }
    return NULL;
};

int server_fd;


void shut_down(int signal)
{
    if (signal == SIGINT)
    {
        log_statement("Received signal to shut down server...");
        close(server_fd);
        destroy_queue(&client_queue);
        log_statement("Server has been shut down.");
        close_logger();
        exit(EXIT_SUCCESS);
    }
};

void start_server(ServerConfig *config)
{
    signal(SIGINT, shut_down);
   
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    
    // Allows reuse of the port
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
   
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    if(inet_pton(AF_INET, config->address, &address.sin_addr) <= 0)
    {
        perror("inet_pton failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_port = htons(config->port);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char log_message[64];
    sprintf(log_message, "Server started on %s:%d", config->address, config->port);
    log_statement(log_message);

    if(listen(server_fd, 10) < 0)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    init_queue(&client_queue, config->max_threads);

    pthread_t thread_pool[config->max_threads];
    for (int i = 0; i < config->max_threads; i++)
    {
        pthread_create(&thread_pool[i], NULL, worker_thread, NULL);
    }

    BZERO(log_message, 64);
    sprintf(log_message, "Thread pool has been created with %d threads.", config->max_threads);
    log_statement(log_message);

    while (1)
    {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);

        if (client_fd < 0)
        {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_address.sin_port);

        char log_message[128];
        sprintf(log_message, "Accepted connection from %s:%d", client_ip, client_port);
        log_statement(log_message);
        
        enqueue(&client_queue, client_fd, client_ip, client_port);
    }
};
