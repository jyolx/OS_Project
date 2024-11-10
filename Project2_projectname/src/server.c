#include "server.h"
#include "http.h"
#include "authentication.h"
#include "logger.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct {
    int *queue;
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
    q->queue = (int *)malloc(size * sizeof(int));
    q->front = 0;
    q->rear = 0;
    q->size = size;
    sem_init(&q->mutex, 0, 1);
    sem_init(&q->items, 0, 0);
    sem_init(&q->spaces, 0, size);
};

void enqueue(ClientQueue *q, int client_fd)
{
    sem_wait(&q->spaces);
    sem_wait(&q->mutex);
    q->queue[q->rear] = client_fd;
    q->rear = (q->rear + 1) % q->size;
    char log_message[64];
    sprintf(log_message, "Client %d has been added to the queue.\n", client_fd);
    log_statement(log_message);
    sem_post(&q->mutex);
    sem_post(&q->items);
};

int dequeue(ClientQueue *q)
{
    sem_wait(&q->items);
    sem_wait(&q->mutex);
    int client_fd = q->queue[q->front];
    q->front = (q->front + 1) % q->size;
    char log_message[64];
    sprintf(log_message, "Client %d has been removed from the queue.\n", client_fd);
    log_statement(log_message);
    sem_post(&q->mutex);
    sem_post(&q->spaces);
    return client_fd;
};

void destroy_queue(ClientQueue *q)
{
    free(q->queue);
    sem_destroy(&q->mutex);
    sem_destroy(&q->items);
    sem_destroy(&q->spaces);
    log_statement("Queue has been destroyed.\n");
};

void *worker_thread(void *arg)
{
    while (1)
    {
        int client_fd = dequeue(&client_queue);
        handle_client(client_fd);
    }
    return NULL;
};

void handle_client(int client_fd)
{
    char buffer[1024];
    recv(client_fd, buffer, sizeof(buffer), 0);
    
    HttpRequest request;
    parse_request(buffer, &request);

    if (authenticate_request(&request)) {
        respond(client_fd, &request);
    } else {
        send(client_fd, "HTTP/1.1 401 Unauthorized\r\n", 26, 0);
    }

    log_request(&request);
    close(client_fd);
    return NULL;
}

void start_server(ServerConfig *config)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(config->address);
    address.sin_port = htons(config->port);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char log_message[64];
    sprintf(log_message, "Server started on %s:%d\n", config->address, config->port);
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

    while (1)
    {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0)
        {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        enqueue(&client_queue, client_fd);
    }

    close(server_fd);
    destroy_queue(&client_queue);
};
