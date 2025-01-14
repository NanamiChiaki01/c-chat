// Peer-to-peer (P2P) code in C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *server_thread(void *arg) {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  // Attaching socket to the port
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Binding the socket
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Listening for connections
  if (listen(server_fd, 3) < 0) {
    perror("Listen");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("Peer listening on port %d\n", PORT);

  while (1) {
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
      perror("Accept");
      close(server_fd);
      exit(EXIT_FAILURE);
    }

    memset(buffer, 0, BUFFER_SIZE);
    read(new_socket, buffer, BUFFER_SIZE);
    if (strcmp(buffer, "exit\n") == 0) {
      printf("Peer disconnected\n");
      close(new_socket);
      break;
    }
    printf("Message from peer: %s", buffer);
    send(new_socket, buffer, strlen(buffer), 0);
    printf("Echoed message to peer\n");
    close(new_socket);
  }

  close(server_fd);
  return NULL;
}

int main() {
  pthread_t server_tid;
  struct sockaddr_in serv_addr;
  char buffer[BUFFER_SIZE] = {0};
  int sock = 0;

  // Start server thread
  if (pthread_create(&server_tid, NULL, server_thread, NULL) != 0) {
    perror("Failed to create server thread");
    exit(EXIT_FAILURE);
  }

  // Client functionality
  while (1) {
    printf("Enter peer IP (or 'exit' to quit): ");
    char peer_ip[BUFFER_SIZE] = {0};
    fgets(peer_ip, BUFFER_SIZE, stdin);
    peer_ip[strcspn(peer_ip, "\n")] = 0; // Remove newline

    if (strcmp(peer_ip, "exit") == 0) {
      printf("Exiting peer-to-peer network\n");
      break;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Socket creation error");
      continue;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, peer_ip, &serv_addr.sin_addr) <= 0) {
      perror("Invalid address/ Address not supported");
      close(sock);
      continue;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("Connection failed");
      close(sock);
      continue;
    }

    printf("Enter message (type 'exit' to quit): ");
    fgets(buffer, BUFFER_SIZE, stdin);
    send(sock, buffer, strlen(buffer), 0);
    if (strcmp(buffer, "exit\n") == 0) {
      printf("Disconnected from peer\n");
      close(sock);
      break;
    }

    memset(buffer, 0, BUFFER_SIZE);
    read(sock, buffer, BUFFER_SIZE);
    printf("Message from peer: %s", buffer);

    close(sock);
  }

  pthread_cancel(server_tid);
  pthread_join(server_tid, NULL);
  return 0;
}
