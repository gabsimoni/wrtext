#include "fmanager.h"
#include <stdio.h>  // Required for FILE, fopen, fread, etc.
#include <stdlib.h>

editor_file *
fmanager_load(char *file_path)
{
    // Open the file in "read binary" mode.
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL) {
        return NULL; // Return NULL if the file doesn't exist or is inaccessible
    }

    // Determine the file size.
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory for the editor_file metadata structure.
    editor_file *f = malloc(sizeof(editor_file));
    if (f == NULL) {
        fclose(fp);
        return NULL;
    }

    // Allocate memory for the actual text/content buffer.
    // We add +1 to the size to leave room for a null terminator ('\0').
    f->content = malloc(fsize + 1);
    if (f->content == NULL) {
        free(f);
        fclose(fp);
        return NULL;
    }

    // Read the file into the buffer and close the file stream.
    fread(f->content, 1, fsize, fp);
    f->content[fsize] = '\0'; // Mark the end of the string
    f->size = fsize;          // Store the size in the structure
    f->path = file_path;      // Store the pointer to the path string

    fclose(fp);
    return f;
}

int
fmanager_save(editor_file *f)
{
    // Safety check: ensure we have a valid structure to save.
    if (f == NULL || f->path == NULL || f->content == NULL) {
        return -1;
    }

    // Open the file in "write binary" mode.
    FILE *fp = fopen(f->path, "wb");
    if (fp == NULL) {
        return -1;
    }

    // Write the buffer content to the disk.
    size_t written = fwrite(f->content, 1, f->size, fp);
    
    fclose(fp);

    // If bytes written matches the size of our buffer, return 0 (success).
    return (written == f->size) ? 0 : -1;
}