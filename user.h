#ifndef USER_H
#define USER_H

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

typedef struct {
    int socket_fd;
    char username[50];
    char chatroom[50];
} ClientInfo;

extern ClientInfo clients[MAX_CLIENTS];
extern int client_count;
extern pthread_mutex_t clients_mutex;

void *handle_client(void *arg);

#endif
