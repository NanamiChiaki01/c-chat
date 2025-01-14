#include <stdio.h>
#include "server.h"
#include "client.h"
#define BUFFER (1024)

int main(int argc, char *argv[])
{
  printf("Starting server...\n");
  current_server = create_server();

  if(!server){
    printf("Server creation failed\nExiting...\n");
    return -1;
  }
  else {
    printf("Server creation success\n");
  }
  
  current_client = create_client();
  if(!client){
    printf("Client creation failed\nExiting...\n");
    close_server(current_server); // shut down the running server
    return -1;
  }else {
    printf("Client creation success\n");
  }


  print("Enter message to send from client to server\n");
  char msg[BUFFER] = fscanf(stdin, "%d");
  client_send(msg, current_server);
  checklog(server);
  
  print("Enter message to send from server to client\n");
  char msg[BUFFER] = fscanf(stdin, "%d");
  checklog(client);

  shutdown(current_server);
  shutdown(current_client);

  return EXIT_SUCCESS;
}
