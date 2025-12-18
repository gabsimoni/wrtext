#include "gui_edit_menu.h"
#include "editor_file.h"
#include "file_dialog.h"
#include "gui_about.h"
#include "gui_edit.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Callback for the "open" action: show open-file dialog.
 *
 * @param action Unused GSimpleAction pointer.
 * @param parameter Unused GVariant pointer.
 * @param user_data Pointer to the GtkApplication passed from the caller.
 */
static void
gui_edit_menu_on_open(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkApplication *app = GTK_APPLICATION(user_data);
	GtkWindow *parent = gtk_application_get_active_window(app);
	char *path = file_dialog_open_file(parent);
	if(path) {
		g_print("Open selected: %s\n", path);
		g_free(path);
	}
}

/**
 * @brief Callback for the "save" action: show save-file dialog and create file.
 *
 * If a path is chosen, an empty file is created (or truncated) so the application
 * can later write into it.
 */
static void
gui_edit_menu_on_save(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkApplication *app = GTK_APPLICATION(user_data);
	GtkWindow *parent = gtk_application_get_active_window(app);
	char *path = file_dialog_save_file(parent, NULL);
	if(path) {
		FILE *f = fopen(path, "w");
		if(f)
			fclose(f);
		g_print("Save selected: %s\n", path);
		g_free(path);
	}
}

GtkWidget *
gui_edit_menu_init(GtkApplication *app)
{

	// Create action
	GSimpleAction *action_randfile = g_simple_action_new("randfile", NULL);
	GSimpleAction *action_open = g_simple_action_new("open", NULL);
	GSimpleAction *action_save = g_simple_action_new("save", NULL);
	GSimpleAction *action_about = g_simple_action_new("about", NULL);
	// Link action to function
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_randfile));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_open));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_save));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_about));
	g_signal_connect(action_randfile, "activate", G_CALLBACK(gui_edit_add_random_file), NULL);

	// Open/save actions. Pass the GtkApplication as user_data so callbacks
	// can obtain the active window with gtk_application_get_active_window(). */
	g_signal_connect(action_open, "activate", G_CALLBACK(gui_edit_menu_on_open), app);
	g_signal_connect(action_save, "activate", G_CALLBACK(gui_edit_menu_on_save), app);
	// Calls the function that activates the about window and passes it the main window
	g_signal_connect(action_about, "activate", G_CALLBACK(gui_about_init),
					 gtk_application_get_active_window(app));
	// Create the two menus
	GMenu *menu_model = g_menu_new();
	GMenu *menu_file_model = g_menu_new();
	GMenu *menu_help_model = g_menu_new();

	GMenuItem *menu_help_menu = g_menu_item_new("Help", NULL);
	GMenuItem *menu_file_menu = g_menu_item_new("File", NULL);
	GMenuItem *item_filerand
		= g_menu_item_new("FileRand", "app.randfile"); // Link option to action
	GMenuItem *item_open = g_menu_item_new("Open...", "app.open");
	GMenuItem *item_save = g_menu_item_new("Save...", "app.save");
	GMenuItem *item_about = g_menu_item_new("About", "app.about");

	// Add File submenu and randfile item
	g_menu_append_item(menu_file_model, item_open);
	g_menu_append_item(menu_file_model, item_save);
	g_menu_append_item(menu_file_model, item_filerand);
	g_menu_item_set_submenu(menu_file_menu, G_MENU_MODEL(menu_file_model));
	g_menu_append_item(menu_model, menu_file_menu);

	// Add Help submenu and About item
	g_menu_append_item(menu_help_model, item_about);
	g_menu_item_set_submenu(menu_help_menu, G_MENU_MODEL(menu_help_model));
	g_menu_append_item(menu_model, menu_help_menu);

	// Set menubar
	gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu_model));

	return NULL;
}
