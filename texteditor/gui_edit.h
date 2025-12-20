#include "editor_file.h"
#include <gtk/gtk.h>

/*!
	@file gui_edit.h
	@brief The files gui_edit.h and gui_edit.c contain the code responsible for managing file tabs and
	the main text area of the application
*/


/*!
	@brief Initializes the main application window responsible for allowing the user to edit
	the code

	@param app Pointer to the GTK application
*/
GtkWidget *gui_edit_init(GtkApplication *app);


/*!
	@brief Adds a file to the list of opened file so that the user may edit it
	@param f Pointer to the file
	@return Returns the ID of the file added, -1 in case of error
*/
editor_file_id gui_edit_add_file(editor_file *f);

/*!
	@brief Function that closes an opened file
	@param id ID of the file to be closed
*/
int gui_edit_close_file(editor_file_id id);


void gui_edit_cleanup();

/*!
	@brief Just a debug function to add a file with random contets
*/
void gui_edit_add_random_file(GSimpleAction *action, GVariant *parameter, gpointer user_data);
