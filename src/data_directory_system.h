/*!
	@file data_directory_system.h
	@brief data_directory_system.h and data_directory_system.c are used for settings saving and
	document state saving. It is used to find and, if necessary, create the directory needed to
	store the application data.

*/

#ifndef DATA_DIRECTORY_SYSTEM
#define DATA_DIRECTORY_SYSTEM

// Name of the application's data folder
#define APPLICATION_NAME "WRText"
#define MAX_PATH_LENGTH 1024

// Takes the filename of a data file and returns the full path including the filename.
// This is set up to work cross-platform on Windows, iOS, Linux, Unix
char *get_data_path(const char *filename);

#endif