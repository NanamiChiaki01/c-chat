#ifndef SERVH
#define SERVH

struct Server
{
  int domain;
  int service;
  int protocol;
  u_long interface;
  int port;
  int backlog;

  struct sockaddr_in address;

  void (*launch) void; // for the user to decide which service should it launch
                      
}

#endif
