/*!
	@brief Structure that represents a file that can be edited and saved by the editor

*/

#pragma once
#include <stdlib.h>

typedef struct{
	char *file_name;
	char *file_path;
	int cursor_position;
	char *contents;
	unsigned long size;
	unsigned long lines;
}editor_file;

typedef size_t editor_file_id;

/*!
	@brief Frees the memory of a editor file
	@param f File to be freed
*/
int editor_file_delete(editor_file *f);

/*!
	@brief Updates the lines field of the file by counting the number of new-lines in the content
	@param f File
*/
int editor_file_update_lines(editor_file *f);

/*!
	@brief Updates the size field of the file by measuring the size of the content
	@param f File
*/
int editor_file_update_size(editor_file *f);
