#include "editor_file.h"
#include "logger.h"

int
editor_file_delete(editor_file *f)
{

	if(f->file_name == NULL || f->file_path == NULL || f->contents == NULL) {
		log_err(__FILE__, "Received corrupt file");
		return -1;
	}

	free(f->file_name);

	free(f->file_path);

	free(f->contents);

	free(f);

	return 0;
}