#ifndef GUI_EDIT_MENU_H
#define GUI_EDIT_MENU_H

#include <gtk/gtk.h>

/*!
 * @file gui_edit_menu.h
 * @brief Top bar inside the WRText window (NOT macOS global menubar).
 */

/*!
 * @brief Creates a top bar widget (hamburger + buttons) and returns it.
 * @param app GTK application
 * @param parent_window The WRText main window (needed to open Settings)
 * @return Widget to be packed at the top of the main window layout
 */
GtkWidget *gui_edit_menu_init(GtkApplication *app, GtkWindow *parent_window);

#endif
