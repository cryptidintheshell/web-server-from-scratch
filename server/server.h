#include "readHTMLfile.h"

int server_fd;
struct sockaddr_in address;
socklen_t addrlen = sizeof(address);
int opt = 1;

int createSocket() {
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("[!] Socket creation failed: ");
		return -1;
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("[!] Socket port failed: ");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);	

	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("[!] Socket binding failed: ");
		return -1;
	}

	return 0;
}

void responseToClient(int *clientSocket, char* body) {
	char* response = malloc(strlen(body) + 256);
	if (!response) { perror("malloc"); return; }

	snprintf(response, strlen(body) + 256,
		"HTTP/1.1 200 OK\r\n"
		"Content-type: text/html\r\n"
		"Content-length: %zu\r\n"
		"\r\n"
		"%s",
		strlen(body), body
	);

	send(*clientSocket, response, strlen(response), 0);
	free(response);
}

void listenToConnections() {
	int clientSocket;

	while(1) {
		if (listen(server_fd, 3) < 0) {
			perror("[!] Listening to connection failed.");
			exit(1);
		}

		if ((clientSocket = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0) {
			perror("[!] Failed to accept client connection.");
			exit(1);
		}

		char* html = readFile("index.html");
		responseToClient(&clientSocket, html);
		free(html);
	}
}