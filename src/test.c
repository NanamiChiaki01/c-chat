#include "../include/server.h"
#include "../include/client.h"
#include <stdio.h>
#include <string.h>
#define BUFLEN (30000)

// this will accept the socket, so we must have server pointer to which server we are trying to accept it from 
void launch(struct Server *server)
{
  char buff[BUFLEN];
  printf("Waiting for connection\n");
  int address_length = sizeof(server->address);
  int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length);
  /* can make another test see if the connection succeeded */
  read(new_socket, buff, BUFLEN);
  printf("%s\n", buff);
  char* hello = "Package hello using HTML";
  write(new_socket, hello, strlen(hello)); /* write to the new socket */
  close(new_socket);

}

int main()
{
  struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 80, 10, launch);
  server.launch(&server);

}
