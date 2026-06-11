// global
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

// server
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 8080


struct ResponseHeaders {
	int status;
	char* reasonPhrase;
	char* contentType;
	int contentLength;
	char* body;
};

char* returnResponse(struct ResponseHeaders *rh) {
  char* response = malloc(strlen(rh->body) + 256);
  if (!response) { perror("malloc: "); return NULL; }

  snprintf(response, strlen(rh->body) + 256,
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %zu\r\n"
    "\r\n"
    rh->status, rh->reasonPhrase, rh->contentType, strlen(response)
  );

  return response;
}