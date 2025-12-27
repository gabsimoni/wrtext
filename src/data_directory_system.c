#include "data_directory_system.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// section for getting save directory
#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

// helper function to create a directory
static void
create_directory(const char *path)
{
#ifdef _WIN32
	CreateDirectoryA(path, NULL);
#else
	mkdir(path, 0755);
#endif
}

// helper function to get the path of a file in the data storage folder
char *
get_data_path(const char *filename)
{
	static char path[MAX_PATH_LENGTH];
	static char fullpath[MAX_PATH_LENGTH];
	memset(path, 0, sizeof(path));
#ifdef _WIN32
	char base[MAX_PATH_LENGTH];
	if(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, base) != S_OK) {
		return NULL;
	}
	snprintf(path, sizeof(path), "%s\\%s\\", base, APPLICATION_NAME);
	create_directory(path);
#elif __APPLE__
	const char *home = getenv("HOME");
	if(!home)
		return NULL;

	snprintf(path, sizeof(path), "%s/Library/Application Support/%s/", home, APPLICATION_NAME);
	create_directory(path);

#else // Linux / Unix
	const char *xdg = getenv("XDG_DATA_HOME");
	if(xdg) {
		snprintf(path, sizeof(path), "%s/%s/", xdg, APPLICATION_NAME);
	} else {
		const char *home = getenv("HOME");
		if(!home)
			return NULL;
		snprintf(path, sizeof(path), "%s/.local/share/%s/", home, APPLICATION_NAME);
	}
	create_directory(path);
#endif
	snprintf(fullpath, sizeof(fullpath), "%s%s", path, filename);

	return fullpath;
}