#include "server/server.h"

int main() {
	printf("\n[+]\tWeb server from scratch\t\t[+]\n\n");

	int serverSocket = createSocket();
	if (serverSocket < 0) printf("Error in creating socket\n");

	printf("[+] Server started.\n\n");
	listenToConnections();

	return 0;
}