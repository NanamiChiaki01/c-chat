#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

struct Server
{
  int domain;
  int service;
  int protocol;
  u_long interface;
  int port;
  int backlog;

  struct sockaddr_in address;

  int socket;

  // some function that should take no param, for user to call
  void (*launch)(struct Server* server); // for the user to decide which service should it launch
                                         // A function pointer to a function that takes a pointer to a server
};

// passing in all these to construct a server fot the users
struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, void(*launch)(struct Server *server));

#endif
