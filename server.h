#ifndef SERVH
#define SERVH

#include <sys/socket.h>
#include <netinet/in.h>

struct Server
{
  int domain;
  int service;
  int protocol;
  u_long interface;
  int port
  int backlog;

  struct sockaddr_in address;

  int socket;

  // some function that should take no param, for user to call
  void (*launch)(void); // for the user to decide which service should it launch
};

// passing in all these to construct a server fot the users
struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, void(*launch)(void));

#endif
