#include "server/server.h"

int main() {
	printf("Web server from scratch\n");

	int serverSocket = createSocket();
	if (serverSocket < 0) printf("Error in creating socket\n");

	printf("Started.\n\n");
	listenToConnections();

	return 0;
}