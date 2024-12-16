#ifndef PEER_H
#define PEER_H
#include "./server.h"

struct Peer
{
  struct Server server;

  int domain;
  int service;
  int protocol;
  int port;
  u_long interface;

  /* To launch the server */
  void (*user_portal)(struct Peer *peer);
  void* (*server_function)(void* arg);
  void* (*client_function)(void* arg);
}

struct Peer peer_constructor(int domain, int service, u_long protocol, int port);

#endif
