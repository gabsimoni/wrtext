#include "gui_edit_menu.h"
#include "editor_file.h"
#include "gui_about.h"
#include "gui_edit.h"
#include "gui_settings.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *
gui_edit_menu_init(GtkApplication *app)
{

	// Create action
	GSimpleAction *action_randfile = g_simple_action_new("randfile", NULL);
	GSimpleAction *action_about = g_simple_action_new("about", NULL);
	GSimpleAction *action_settings = g_simple_action_new("settings", NULL);
	// Link action to function
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_randfile));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_about));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_settings));

	g_signal_connect(action_randfile, "activate", G_CALLBACK(gui_edit_add_random_file), NULL);
	// Calls the function that activates the about window and passes it the main window
	g_signal_connect(action_about, "activate", G_CALLBACK(gui_about_init),
					 gtk_application_get_active_window(app));
	g_signal_connect(action_settings, "activate", G_CALLBACK(gui_settings_init),
					 gtk_application_get_active_window(app));

	// Create the two menus
	GMenu *menu_model = g_menu_new();	   // main
	GMenu *menu_file_model = g_menu_new(); // file -> rand
	GMenu *menu_help_model = g_menu_new(); // help -> about
	GMenu *menu_edit_model = g_menu_new(); // edit -> settings

	GMenuItem *menu_help_menu = g_menu_item_new("Help", NULL);
	GMenuItem *menu_file_menu = g_menu_item_new("File", NULL);
	GMenuItem *menu_edit_menu = g_menu_item_new("Edit", NULL);

	// Items
	GMenuItem *item_filerand
		= g_menu_item_new("FileRand", "app.randfile"); // Link option to action
	GMenuItem *item_about = g_menu_item_new("About", "app.about");
	GMenuItem *item_settings = g_menu_item_new("Settings", "app.settings");

	// Add File submenu and randfile item
	g_menu_append_item(menu_file_model, item_filerand);
	g_menu_item_set_submenu(menu_file_menu, G_MENU_MODEL(menu_file_model));
	g_menu_append_item(menu_model, menu_file_menu);

	// Add Edit submenu and Settings item
	g_menu_append_item(menu_edit_model, item_settings);
	g_menu_item_set_submenu(menu_edit_menu, G_MENU_MODEL(menu_edit_model));
	g_menu_append_item(menu_model, menu_edit_menu);

	// Add Help submenu and About item
	g_menu_append_item(menu_help_model, item_about);
	g_menu_item_set_submenu(menu_help_menu, G_MENU_MODEL(menu_help_model));
	g_menu_append_item(menu_model, menu_help_menu);

	// Set menubar
	gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu_model));

	return NULL;
}