#include <stdint.h>

/*!
	@file document_save_system.h
	@brief document_save_system.h and document_save_system.c handle operations involed with
	loading and saving the state of documents. Entries are written in a custom binary format.

*/


#ifndef DOCUMENT_SAVE_SYSTEM
#define DOCUMENT_SAVE_SYSTEM

#define DOCUMENT_STORAGE_FILE_NAME "sessionData.bin"

typedef struct {
	char *path; // The path of the file is stored dynamically. Remember to free memory after using
	int64_t lastEdited;
	uint8_t language; // one byte to represent the choice of program language (0 for unknown, 1 for
					  // c, etc)
	uint8_t encoding; // similar to above, choices being 0 for unknown, etc

	// cursor position
	uint32_t cursorPosition; // max is roughly 2gb
	uint32_t selectRange; // the number of characters selected
	uint32_t scrollPosition;
} Document;


// Takes the input of a file's path and returns a document from the data file,
// or an empty empty struct if no data is found
Document get_document(const char *filepath); // remember to free path memory

// Takes the input of a document struct and saves it to the data file,
// if an entry exists, it will be overwritten, otherwise the entry is appended
int save_document(Document);

#endif