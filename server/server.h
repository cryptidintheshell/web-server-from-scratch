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

      char *html = getPage(path);
      if (!html) {
        // send 404
        char *notFound = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(clientSocket, notFound, strlen(notFound), 0);
      } else {
        responseToClient(&clientSocket, html);
        free(html);
      }

      close(clientSocket); // critical — tells browser the response is done
    }
}