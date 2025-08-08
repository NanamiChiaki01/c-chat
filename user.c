#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "user.h"

ClientInfo clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

static int is_valid_username(const char *username) {
    if (strlen(username) == 0) return 0;
    for (int i = 0; username[i]; i++) {
        if (!isalnum(username[i])) return 0;
    }
    return 1;
}

static int is_username_taken(const char *username) {
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].username, username) == 0) {
            return 1;
        }
    }
    return 0;
}

static int find_client_index_by_sock(int sock) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket_fd == sock) {
            return i;
        }
    }
    return -1;
}

static void broadcast_to_chatroom(const char *chatroom, const char *message) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (strcmp(clients[i].chatroom, chatroom) == 0) {
            send(clients[i].socket_fd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    int sock = *((int *)arg);
    free(arg);
    char buffer[BUFFER_SIZE];
    char username[50];
    char chatroom[50];

    // Receive username
    memset(username, 0, sizeof(username));
    if (recv(sock, username, sizeof(username), 0) <= 0) {
        close(sock);
        return NULL;
    }

    // Validate username
    pthread_mutex_lock(&clients_mutex);
    if (!is_valid_username(username)) {
        send(sock, "Invalid username\n", 17, 0);
        pthread_mutex_unlock(&clients_mutex);
        close(sock);
        return NULL;
    }
    if (is_username_taken(username)) {
        send(sock, "Username taken\n", 15, 0);
        pthread_mutex_unlock(&clients_mutex);
        close(sock);
        return NULL;
    }
    send(sock, "OK\n", 3, 0);
    pthread_mutex_unlock(&clients_mutex);

    // Receive chatroom name
    memset(chatroom, 0, sizeof(chatroom));
    if (recv(sock, chatroom, sizeof(chatroom), 0) <= 0) {
        close(sock);
        return NULL;
    }

    // Add client to list
    pthread_mutex_lock(&clients_mutex);
    clients[client_count].socket_fd = sock;
    strcpy(clients[client_count].username, username);
    strcpy(clients[client_count].chatroom, chatroom);
    client_count++;
    pthread_mutex_unlock(&clients_mutex);

    // Announce join
    char join_msg[BUFFER_SIZE];
    snprintf(join_msg, sizeof(join_msg), "📢 %s has joined the chatroom.\n", username);
    broadcast_to_chatroom(chatroom, join_msg);

    // Listen for messages
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            printf("Client %s disconnected.\n", username);
            char dc_msg[BUFFER_SIZE];
            snprintf(dc_msg, sizeof(dc_msg), "📢 %s has left the chatroom.\n", username);
            broadcast_to_chatroom(chatroom, dc_msg);
            break;
        }

        buffer[bytes_received] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            printf("Client %s exited the chatroom.\n", username);
            char exit_msg[BUFFER_SIZE];
            snprintf(exit_msg, sizeof(exit_msg), "📢 %s has left the chatroom.\n", username);
            broadcast_to_chatroom(chatroom, exit_msg);
            break;
        }

        // Handle commands
        if (strncmp(buffer, "/join ", 6) == 0) {
            char new_chatroom[50];
            sscanf(buffer + 6, "%49s", new_chatroom);

            pthread_mutex_lock(&clients_mutex);
            int idx = find_client_index_by_sock(sock);
            if (idx != -1) {
                strcpy(clients[idx].chatroom, new_chatroom);
            }
            pthread_mutex_unlock(&clients_mutex);

            snprintf(buffer, sizeof(buffer), "You joined chatroom: %s\n", new_chatroom);
            send(sock, buffer, strlen(buffer), 0);
            continue;
        }
        else if (strncmp(buffer, "/leave", 6) == 0) {
            snprintf(buffer, sizeof(buffer), "You left chatroom: %s\n", chatroom);
            send(sock, buffer, strlen(buffer), 0);

            pthread_mutex_lock(&clients_mutex);
            int idx = find_client_index_by_sock(sock);
            if (idx != -1) {
                strcpy(clients[idx].chatroom, "");
            }
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }
        else if (strncmp(buffer, "/list", 5) == 0) {
            snprintf(buffer, sizeof(buffer), "Current chatroom: %s\n", chatroom);
            send(sock, buffer, strlen(buffer), 0);
            continue;
        }

        // Broadcast regular chat message
        char formatted[BUFFER_SIZE];
        // Ensure message length won't overflow formatted buffer
snprintf(formatted, sizeof(formatted), "[%s]: %.*s", username, (int)(sizeof(formatted) - strlen(username) - 5), buffer);
        broadcast_to_chatroom(chatroom, formatted);
    }

    // Remove client from list
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket_fd == sock) {
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(sock);
    return NULL;
}
