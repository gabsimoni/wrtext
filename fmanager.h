/*!
	@file fmanager.h
	@brief fmanager.h and fmanager.c contain the code responsible for loading files from the file
	system and interfacing with the document state database to create editor files that can be
	viewed and edited

*/
#include "editor_file.h"

editor_file *fmanager_load(char *file_path);

int fmanager_save(editor_file *f);