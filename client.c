// Client code in C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int client() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connecting to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
  return sock;
}

void send_msg(int sock){
  /*prompt for input and loop*/
    char *message = "Hello from client";
    char buffer[BUFFER_SIZE] = {0};

    send(sock, message, strlen(message), 0);
    printf("Message sent to server\n");
    read(sock, buffer, BUFFER_SIZE);
    printf("Message from server: %s\n", buffer);

    //close(sock);
    return;
}
