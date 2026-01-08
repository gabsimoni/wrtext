#include <gtk/gtk.h>


/*!
	@file gui_settings.h
	@brief The files gui_settings.h and gui_settings.c contain the code responsible for creating the
	settings window and modifying the application settings based on the UI state
*/

/*!
 * @brief Function called whenever the settings panel has to be opened. It re-creates the settings
   window each time. The reason is that recreating the window each time appears to be the only way
   to re-center the window across all platforms. This function is connected to the menu button which 
   triggers the opening of the settings panel.
 */
void gui_settings_init(GSimpleAction *action, GVariant *parameter, gpointer user_data);
