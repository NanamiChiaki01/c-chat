#include "../include/peer.h"
#include "../include/client.h"
#include "../include/server.h"

void handle_conn(int sock_fd)
{
	char buffer[BUFLEN];

}

struct Peer peer_constructor(int domain, int protocol, u_long interface, int port)
{
		struct Peer peer;
		peer.domain = domain;
		peer.protocol = protocol;
		peer.interface = interface;
		peer.port = port;

		/* set up server for this peer, it will continuously listening for connection */
		peer.server = server_constructor(domain, protocol, interface, port);

		return peer;
}


