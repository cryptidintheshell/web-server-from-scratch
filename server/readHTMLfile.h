#include "headers.h"

char* readFile(char* path, long *fileSize) {
	FILE *fileptr = fopen(path, "rb");
	if (!fileptr) {
		perror("fopen: "); 
		return NULL;
	}

	fseek(fileptr, 0, SEEK_END);
	*fileSize = ftell(fileptr);
	rewind(fileptr);

	char* content = calloc(fileSize + 1, 1);
	if (!content) {
		perror("calloc: ");
		fclose(fileptr);
		return NULL;
	}

	fread(content, 1, *fileSize, fileptr);

	fclose(fileptr);
	return content;
}

int checkIfPathExists(char* path) {
	DIR* dir = opendir(path);
	if (dir) return 200;									// found
	else if (ENOENT == errno) return 404; // not found
	else return 505; 										// opendir error
}

char* getContentType(char* path) {
  char *ext = strchr(path, '.');
  if (!ext) return "text/html";
  if (strcmp(ext, ".css") == 0)  return "text/css";
  if (strcmp(ext, ".js") == 0)   return "application/javascript";
  if (strcmp(ext, ".png") == 0)  return "image/png";
  if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)  return "image/jpeg";
  if (strcmp(ext, ".ico") == 0)  return "image/x-icon";
  return "text/html";
}

char* getPage(char* requestPath, long *fileSize) {
  char filepath[512];

  char *ext = strchr(requestPath, '.');
  if (strcmp(requestPath, "/") == 0 || ext == NULL) {
    if (strcmp(requestPath, "/") == 0) strcpy(filepath, "pages/index.html");
    else snprintf(filepath, sizeof(filepath), "pages%s/index.html", requestPath);
  } else snprintf(filepath, sizeof(filepath), "pages%s", requestPath);

  printf("[+] Response path: %s\n", filepath);
  if (ext) printf("[+] File type: %s\n\n", ext + 1); // +1 to skip the dot

  FILE *f = fopen(filepath, "r");
  if (!f) {
  	printf("File not found\n");
  	return NULL;
  }

  return readFile(filepath, fileSize);
}