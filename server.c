#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10


// Global variables to manage clients
// Note: In a real-world application, you would want to use a more sophisticated data structure
// to manage clients, such as a linked list or a dynamic array.
// Here, we use a fixed-size array for simplicity.
int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// prototypes
void *handle_client(void *arg);
void broadcast_message(const char *message, int sender_fd);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    pthread_t tid;

    // 1. Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // multiple clients
    while(1){
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { // on success, return fd for the accepted socket
            perror("Accept failed");
            continue;
        }

        // Add client to list
        pthread_mutex_lock(&clients_mutex);
        if (client_count >= MAX_CLIENTS) {
            char *msg = "Server full. Try again later.\n";
            send(new_socket, msg, strlen(msg), 0);
            close(new_socket);
        } else {
            clients[client_count++] = new_socket;

            // Create a new thread for the client
            int *pclient = malloc(sizeof(int));
            *pclient = new_socket;
            pthread_create(&tid, NULL, handle_client, pclient);
            pthread_detach(tid);
            printf("Client %d connected.\n", new_socket);

            // Broadcast join message
            char join_msg[BUFFER_SIZE] = {0};
            snprintf(join_msg, sizeof(join_msg), "Client %d has joined the chatroom.\n", new_socket);
            broadcast_message(join_msg, -1);  // -1 means send to all

        }
        pthread_mutex_unlock(&clients_mutex);
        
    }
    close(server_fd);
    return 0;
}

void *handle_client(void *arg) {
    int sock = *(int *)arg;
    free(arg);
    char buffer[BUFFER_SIZE] = {0}; // buffer to store incoming messages
    char message[BUFFER_SIZE+64];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // clear the buffer
        int valread = read(sock, buffer, BUFFER_SIZE); // read returns 0 if buffer overflows
        if (valread <= 0) {
            printf("Client %d disconnected.\n", sock);
            snprintf(message, sizeof(message), "Client %d has left the chatroom.\n", sock);
            broadcast_message(message, -1);
            break;
        }

        snprintf(message, sizeof(message), "Client %d: %s", sock, buffer);
        printf("%s", message);
        broadcast_message(message, sock);
    }

    // Remove client from list
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i] == sock) {
            clients[i] = clients[client_count - 1]; // Move last client here
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(sock);
    return NULL;
}

// using a mutex to protect access to the clients array
void broadcast_message(const char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i] != sender_fd || sender_fd == -1) { // if sender_fd is -1, send to all clients
            // Send message to all clients except the sender
            send(clients[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex); // lock & unlock the clients array avoid competition between threads
}