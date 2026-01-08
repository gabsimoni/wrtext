

/*!
	@file settings.h
	@brief The files settings.h and settings.c contain the code responsible for reading and modifying the runtime settings of the program
*/

typedef struct{
	int textwrap;
	int whitespace;
	int linenums;
	int darkmode;
	char font[128];
}settings_state;


/*!
	@brief Returns a copy of the application settings
*/
settings_state settings_get();


/*!
	@brief Copies the settings passed as a parameter to the application settings
*/
void settings_set(settings_state *s);

