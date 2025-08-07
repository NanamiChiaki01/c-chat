#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void *handle_client(void *arg);

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept loop
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        printf("New client connected (fd = %d)\n", client_fd);

        int *client_sock = malloc(sizeof(int));
        *client_sock = client_fd;

        if (pthread_create(&thread_id, NULL, handle_client, client_sock) != 0) {
            perror("pthread_create failed");
            free(client_sock);
            continue;
        }

        pthread_detach(thread_id); // Don't require join
    }

    close(server_fd);
    return 0;
}

// Thread function for handling each client
void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    free(arg);

    char buffer[BUFFER_SIZE];
    const char *reply = "Message received";

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (valread <= 0) {
            printf("Client (fd=%d) disconnected.\n", client_fd);
            break;
        }

        buffer[valread] = '\0';
        printf("Client (fd=%d) says: %s\n", client_fd, buffer);

        send(client_fd, reply, strlen(reply), 0);
    }

    close(client_fd);
    return NULL;
}
