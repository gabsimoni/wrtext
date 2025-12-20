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
}editor_file;

typedef size_t editor_file_id;

/*!
	@brief Frees the memory of a editor file
	@param f File to be freed
*/
int editor_file_delete(editor_file *f);