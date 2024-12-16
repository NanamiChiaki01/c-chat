#include "../include/server.h"
#include <stdlib.h>

// Server constructor
struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, void(*launch)(struct Server* server))
{
  struct Server server;

  server.domain = domain;
  server.service = service;
  server.protocol = protocol;
  server.interface = interface;
  server.port = port;
  server.backlog =  backlog;
  
  server.address.sin_family = domain;
  server.address.sin_port = htons(port); // convert digital port to meaningful network port
  server.address.sin_addr.s_addr = htonl(interface); // convert to long interface to the network

  // create socket connection to the network
  // server.socket is a socket fd
  server.socket = socket(domain, service, protocol);
  if (server.socket == 0)
  {
    perror("Failed to connect socket...\n");
    exit(1);
  }

  // need to cast the socket address created to a sockaddr pointer for reference
  if(bind(server.socket, (struct sockaddr*)&server.address, sizeof(server.address))< 0 )
  {
    perror("Failed to bind socket...\n");
    exit(1);
  }

  // tell the server that it should be listening
  // the backlog defines how many connections are allowed to be waiting
  // this will check error
  if(listen(server.socket, server.backlog)<0)
  {
    perror("Failed to listen ... \n");
    exit(1);
  }

  server.launch = launch;

  return server;
}
