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

void responseToClient(int *clientSocket, char *path) {
	int status = 200;
	char *reasonPhrase = "OK";
	long fileSize = 0;

	char *q = strchr(path, '?');
	if (q) *q = '\0'; // truncate at '?'

	char *body = getPage(path, &fileSize);
	if (!body) {
		status = 404;
		reasonPhrase = "Not Found";
		body = strdup("Not Found"); // heap allocated so free() is safe
		fileSize = strlen(body);
	}

	char *contentType = getContentType(path);

	struct ResponseHeaders rh = {status, reasonPhrase, contentType, fileSize};
	char headers[256];
	returnResponse(&rh, headers, sizeof(headers));

	send(*clientSocket, headers, strlen(headers), 0); // strlen fine, headers are text
	send(*clientSocket, body, fileSize, 0);            // fileSize correct for binary

	free(body);
}

void listenToConnections() {
	int clientSocket;
	struct timeval timeout = {5, 0}; // 5 seconds
	setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));


	if (listen(server_fd, 3) < 0) {
		perror("[!] Listening failed.");
		exit(1);
	}

	while (1) {
		if ((clientSocket = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0) {
			perror("[!] Failed to accept client connection.");
			continue;
		}

		char requestBuffer[1024] = {0};
		read(clientSocket, requestBuffer, sizeof(requestBuffer) - 1);

		char method[8], path[256], version[16];
		sscanf(requestBuffer, "%s %s %s", method, path, version);
		printf("[+] Method: %s\n", method);
		printf("[+] Requested path: %s\n\n", path);

		if (fork() == 0) {
			responseToClient(&clientSocket, path);
			close(clientSocket);
			exit(0);
		}

		close(clientSocket);
	}
}
