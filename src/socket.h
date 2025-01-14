#ifndef SOCK_H
#define SOCK_H

/*return the last message received by this sock*/
void* checklog(int socketfd);

/*close the socket fd*/
void shutdown(int socketfd);

#endif
