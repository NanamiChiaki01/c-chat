#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    const char *reply = "Message received";

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
    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client
    client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (client_fd < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");

    const char *welcome = "Welcome to the server!";
    int sent = send(client_fd, welcome, strlen(welcome), 0);
    if (sent < 0) {
        perror("send failed");
    } else {
        printf("Sent welcome message (%d bytes)\n", sent);
    }



    // Receive and reply loop
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (valread <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Client says: %s\n", buffer);

        // Send reply
        send(client_fd, reply, strlen(reply), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
