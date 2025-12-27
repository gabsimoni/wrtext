#include "document_save_system.h"
#include "data_directory_system.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structure to store the document entries cleanly
typedef struct {
	uint64_t next;		 // how many bytes below the next document is stored
	uint16_t path_length; // length of path in bytes (max is 65536)
} DocumentHeader;

// Helper function to get the offset of a document with a certain path value
long
get_document_offset(FILE *f, const char *path)
{
	rewind(f); // sets the offset back to the beginning of file

	while(1) {
		uint64_t record_size;
		long record_start = ftell(f);
		size_t read = fread(&record_size, sizeof(record_size), 1, f);
		if(read != 1) {
			printf("No data found\n");
			return -1; // EOF or error
		}

		uint16_t path_length;
		fread(&path_length, sizeof(path_length), 1, f);

		char read_path[path_length + 1];
		if(path_length <= 0) {
			// skip malformed entry
			fseek(f, record_start + record_size, SEEK_SET);
			continue;
		}

		fread(read_path, 1, path_length, f);
		read_path[path_length] = '\0';
		if(strcmp(read_path, path) == 0) {
			printf("Found at offset %ld\n", record_start);
			return record_start;
		}

		// jump to next record
		fseek(f, record_start + record_size, SEEK_SET);
	}
}

Document
get_document(const char *path)
{
	Document document = { 0 };
	short path_length = strlen(path);
	document.path = malloc(path_length + 1);
	memcpy(document.path, path, path_length);
	document.path[path_length] = '\0';
	char *filename = get_data_path(DOCUMENT_STORAGE_FILE_NAME);
	FILE *f = fopen(filename, "rb");
	if(!f)
		return document;

	// jump to spot
	long offset = get_document_offset(f, document.path);
	if(offset >= 0) {
		fseek(f, offset + sizeof(uint16_t) + sizeof(uint64_t) + path_length, SEEK_SET);
	} else {
		printf("No document found, returing empty document\n");
		return document;
	}
	fread(&document.lastEdited, sizeof(document.lastEdited), 1, f);
	fread(&document.language, sizeof(document.language), 1, f);
	fread(&document.encoding, sizeof(document.encoding), 1, f);
	fread(&document.cursorPosition, sizeof(document.cursorPosition), 1, f);
	fread(&document.selectRange, sizeof(document.selectRange), 1, f);
	fread(&document.scrollPosition, sizeof(document.scrollPosition), 1, f);

	fclose(f);
	return document;
}

int
save_document(Document document)
{
	DocumentHeader header;
	header.path_length = strlen(document.path);
	char *filename = get_data_path(DOCUMENT_STORAGE_FILE_NAME);
	printf("data file: %s\n", filename);
	FILE *f = fopen(filename, "r+b");
	long offset = 0;
	if(!f) {
		f = fopen(filename, "w+b"); // creates the file
	} else {						// get location of the target file in the existing data file
		offset = get_document_offset(f, document.path);
	}

	header.path_length = (uint16_t)strlen(document.path);
	header.next = sizeof(header.path_length) + header.path_length + sizeof(header.next)
				  + sizeof(document.lastEdited) + sizeof(document.language)
				  + sizeof(document.encoding) + sizeof(document.cursorPosition)
				  + sizeof(document.selectRange) + sizeof(document.scrollPosition);

	// jump to spot
	if(offset >= 0) {
		fseek(f, offset, SEEK_SET);
	} else {
		printf("No data found, appen entry\n"); // jump to end
		fseek(f, 0, SEEK_END);
	}
	// write to file
	fwrite(&header.next, sizeof(header.next), 1, f);
	fwrite(&header.path_length, sizeof(header.path_length), 1, f);
	fwrite(document.path, 1, header.path_length, f);

	fwrite(&document.lastEdited, 1, sizeof(document.lastEdited), f);
	fwrite(&document.language, 1, sizeof(document.language), f);
	fwrite(&document.encoding, 1, sizeof(document.encoding), f);
	fwrite(&document.cursorPosition, 1, sizeof(document.cursorPosition), f);
	fwrite(&document.selectRange, 1, sizeof(document.selectRange), f);
	fwrite(&document.scrollPosition, 1, sizeof(document.scrollPosition), f);
	fclose(f);
	return 0;
}
