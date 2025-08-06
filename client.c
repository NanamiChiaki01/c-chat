#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

// Global variables
int sock; 

// function prototypes
void *receive_messages(void* arg);

int main() {
    struct sockaddr_in serv_addr;
    char message[BUFFER_SIZE] = {0};
    pthread_t recv_thread;

    // 1. Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // 2. Setup server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 3. Convert IP
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // 4. Connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to chatroom.\n");
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_detach(recv_thread); // Detach thread for independent execution

    // 5. Communicate
    while (1) {
        printf("> ");
        fflush(stdout); // Ensure prompt is shown immediately
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) break;
        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE] = {0};
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            printf("Disconnected from server.\n");
            exit(0);
        }
        printf("\n%s> ", buffer);
        fflush(stdout); // clear the line for new input
    }
    return NULL;
}