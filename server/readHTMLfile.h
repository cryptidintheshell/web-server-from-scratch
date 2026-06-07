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