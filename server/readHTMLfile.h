#include "headers.h"

char* readFile(char* path) {
	FILE *fileptr = fopen(path, "r");
	if (!fileptr) {
		perror("fopen: "); 
		return NULL;
	}

	fseek(fileptr, 0, SEEK_END);
	long fileSize = ftell(fileptr);
	rewind(fileptr);

	char* content = calloc(fileSize + 1, 1);
	if (!content) {
		perror("calloc: ");
		fclose(fileptr);
		return NULL;
	}

	fread(content, 1, fileSize, fileptr);

	fclose(fileptr);
	return content;
}

int checkIfPathExists(char* path) {
	DIR* dir = opendir(path);
	if (dir) return 0;									// found
	else if (ENOENT == errno) return 1; // not found
	else return -1; 										// opendir error
}

char* getPage(char* requestPath) {
  char filepath[512];

  if (strcmp(requestPath, "/") == 0) strcpy(filepath, "pages/index.html"); // default to main page
	else snprintf(filepath, sizeof(filepath), "pages%s/index.html", requestPath); // default to index.html

	printf("[+] Response path: %s\n\n", filepath);	

  FILE *f = fopen(filepath, "r");
  if (!f) return NULL; // 404

  return readFile(filepath);
}