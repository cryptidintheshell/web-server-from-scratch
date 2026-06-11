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

void responseToClient(int *clientSocket, char* path) {
	int status = 200;
	char* reasonPhrase = "OK";
	char* contentType;
	long contentLength = 0;
	char* body = getPage(path, &contentLength);
 
	if (body == NULL) { // not found
		status = 404;
		reasonPhrase = "Not Found";
		body = getPage("/404", &contentLength);
	} else contentType = getContentType(path);

	struct ResponseHeaders rh = {status, reasonPhrase, contentType, contentLength, body};
	char* response = returnResponse(&rh);

	send(*clientSocket, response, contentLength, 0);
	send(*clientSocket, body, contentLength, 0);
	free(response);
	free(body);
}

void listenToConnections() {
  int clientSocket;

  if (listen(server_fd, 3) < 0) {  // moved outside loop
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

    responseToClient(&clientSocket, path);
    close(clientSocket);
  }
}