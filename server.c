#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg);
void broadcast_message(char *message, int sender_fd);

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    pthread_t thread_id;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Broadcast server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        pthread_mutex_lock(&clients_mutex);
        if (client_count >= MAX_CLIENTS) {
            printf("Max clients reached. Connection refused.\n");
            close(client_fd);
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }
        clients[client_count++] = client_fd;
        pthread_mutex_unlock(&clients_mutex);

        printf("Client connected: fd = %d\n", client_fd);

        int *client_sock = malloc(sizeof(int));
        *client_sock = client_fd;
        pthread_create(&thread_id, NULL, handle_client, client_sock);
        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    free(arg);

    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + 50];  // To include sender info

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (valread <= 0) {
            printf("Client disconnected: fd = %d\n", client_fd);
            break;
        }

        buffer[valread] = '\0';
        printf("Received from %d: %s\n", client_fd, buffer);

        snprintf(message, sizeof(message), "Client %d: %s", client_fd, buffer);
        broadcast_message(message, client_fd);
    }

    close(client_fd);

    pthread_mutex_lock(&clients_mutex);
    // Remove client from list
    for (int i = 0; i < client_count; ++i) {
        if (clients[i] == client_fd) {
            clients[i] = clients[client_count - 1];  // Replace with last
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    return NULL;
}

void broadcast_message(char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        int client = clients[i];
        if (client != sender_fd) {
            send(client, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}
