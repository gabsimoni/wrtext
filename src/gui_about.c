#include "gui_about.h"
#include "logger.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>


void gui_about_init(GSimpleAction *action, GVariant *parameter, gpointer user_data){

	// Create about window
	GtkWindow *main_window = GTK_WINDOW(user_data);
	GtkWidget *about_window = gtk_window_new();
	gtk_window_set_transient_for(GTK_WINDOW(about_window), GTK_WINDOW(main_window)); // Ties the two windows
	gtk_window_set_modal(GTK_WINDOW(about_window), TRUE); // Makes the edit window unusable
	gtk_window_set_title(GTK_WINDOW(about_window), "About");
	gtk_window_set_resizable(GTK_WINDOW(about_window), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(about_window), 200, 150);

	// Create text
	char aboutstr[50] = "WRText Text Editor\n\nVersion ";
	strcat(aboutstr, VER);

	GtkWidget *label = gtk_label_new(aboutstr);

	gtk_window_set_child(GTK_WINDOW(about_window),GTK_WIDGET(label));
	// Show them
	gtk_window_present(GTK_WINDOW(about_window));

	log_info(__FILE__, "Opened about window");
}