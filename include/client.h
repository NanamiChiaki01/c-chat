#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>


struct Client
{
  int socket;
  int domain;
  int service;
  int protocol;
  int port;
  u_long interface;
  /* method to reach another server */
  char * (*request)(struct Client *client, char *server_ip, void *request, unsigned long size);
}

struct Client client_constructor(int domain, int service, int protocol, int port, u_long interface);

#endif
