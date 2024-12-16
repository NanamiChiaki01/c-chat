#include "../include/client.h"
#define BUFLEN (30000)

/* default request function */
char* request(struct Client* client, char* server_ip, void* request, unsigned long size);

struct Client client_constructor(int domain, int service, int protocol, int port, u_long interface)
{
  struct Client client;
  client.domain = domain;
  client.service = service;
  client.protocol = protocol;
  client.port = port;
  client.interface = interface;

  client.socket = socket(domain, service, protocol);
  client.request = request
  return client;
}

/* private member methods: only accessible within this file, internal changes does not affect the whole file */
char* request(struct Client* client, char* server_ip, void* request, unsigned long size)
{
  struct sockaddr_in server_address;
  server_address.sin_family = client->protocol;
  server_address.sin_port = htons(client->port);
  server_address.sin_addr.s_addr = (int)client->domain;
  /* convert string to network address structure in the address family AF
   * here, server_ip is the character string src, will be converted using the AF of client->domain to &server_address.sin_addr 
   * Convert IPv4 and IPv6 addresses from text to binary */
  if (inet_pton(client->domain, server_ip, &server_address.sin_addr)<=0)
  {
    perror("\nInvalid address / Address not supported\n");
    exit(-1);
  }

  if (connect(client->socket, (struct sockaddr*)&server_address, sizeof(server_address))<0)
  {
    perror("\nConnection Failed\n");
    exit(-1);
  }

  send(client->socket, request, size, 0);
  char* response = malloc(BUFLEN);
  read(client->socket, response, BUFLEN-1);
  return response;
}

