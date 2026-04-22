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
	long contentLength;
};

char* returnResponse(struct ResponseHeaders *rh, char* buffer, size_t bufferSize) {
	snprintf(buffer, bufferSize,
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %ld\r\n"
		"\r\n",
		rh->status, rh->reasonPhrase, rh->contentType, rh->contentLength
	);
	return buffer;
}
