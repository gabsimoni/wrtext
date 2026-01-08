#include "gui_settings.h"
#include "logger.h"
#include "settings.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *settings_window;
GtkWindow *parent_window;
GtkWidget *font_button;

/*
 * When this variable is set to "1" changes to the settings won't be applied.
 This is used for programmatically toggling buttons, avoiding triggering settings changes when
 checkboxes are manually set through code for example.
 */
int updating_checkboxes_through_code = 0;

// This function is called whenever one of the checkboxes is toggled
static void
on_checkboxes_toggled(GtkButton *button, gpointer user_data)
{

	if(updating_checkboxes_through_code)
		return;

	// loads settings and modifies them
	settings_state s = settings_get();

	if(!g_strcmp0("wrap", (char *)user_data))
		s.textwrap = !s.textwrap;
	if(!g_strcmp0("linenums", (char *)user_data))
		s.linenums = !s.linenums;
	if(!g_strcmp0("whitespace", (char *)user_data))
		s.whitespace = !s.whitespace;
	if(!g_strcmp0("darkmode", (char *)user_data))
		s.darkmode = !s.darkmode;

	settings_set(&s);

	log_info(__FILE__, "toggled: %s", (char *)user_data);
}

// This function is called whenever the font dialog is closed because a font was chosen
static void
on_font_dialog_response(GtkDialog *dialog, int response_id, gpointer user_data)
{
	if(response_id == GTK_RESPONSE_OK) {
		// Get font description from dialog
		PangoFontDescription *font;
		font = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(dialog));

		if(font) {
			// Saves new font to the settings
			char *font_str = pango_font_description_to_string(font);
			log_info(__FILE__, "Selected font: %s\n", font_str);

			settings_state s = settings_get();
			strcpy(s.font, font_str);
			g_free(font_str);
			settings_set(&s);
			// Update label on the font button
			gtk_button_set_label(GTK_BUTTON(font_button), s.font);
		} else {
			// should handle this
			log_err(__FILE__, "Font not found");
		}
	} else {
		// should handle this
		log_err(__FILE__, "Font not found");
	}

	gtk_window_destroy(GTK_WINDOW(dialog));
}

// This function is called whenever the font button is clicked
static void
on_choose_font_clicked(GtkButton *button, gpointer user_data)
{
	GtkWindow *parent = GTK_WINDOW(user_data);
	GtkWidget *dialog;
	// Creates new font dialog
	dialog = gtk_font_chooser_dialog_new("Font used:", parent);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(settings_window));

	g_signal_connect(dialog, "response", G_CALLBACK(on_font_dialog_response), user_data);

	gtk_window_present(GTK_WINDOW(dialog));
}

void
gui_settings_init(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{

	settings_window = gtk_window_new();
	// Create settings window
	parent_window = GTK_WINDOW(user_data);

	gtk_window_set_transient_for(GTK_WINDOW(settings_window),
								 GTK_WINDOW(parent_window)); // Ties the two windows
	gtk_window_set_modal(GTK_WINDOW(settings_window), TRUE); // Makes the edit window unusable
	gtk_window_set_title(GTK_WINDOW(settings_window), "Settings");
	gtk_window_set_resizable(GTK_WINDOW(settings_window), TRUE);

	// Main box containing all the options
	GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	// Set margins
	gtk_widget_set_margin_top(main_box, 12);
	gtk_widget_set_margin_bottom(main_box, 12);
	gtk_widget_set_margin_start(main_box, 12);
	gtk_widget_set_margin_end(main_box, 24);

	gtk_window_set_child(GTK_WINDOW(settings_window),
						 main_box); // The box is the child of the window

	// Checkboxes
	GtkWidget *checkbox_wrap = gtk_check_button_new_with_label("Text-wrapping");
	GtkWidget *checkbox_whitespace = gtk_check_button_new_with_label("Whitespace rendering");
	GtkWidget *checkbox_linenums = gtk_check_button_new_with_label("Line numbers");
	GtkWidget *checkbox_darkmode = gtk_check_button_new_with_label("Dark mode");

	// Labels
	GtkWidget *label_editing = gtk_label_new("Text Editing");
	GtkWidget *label_appearance = gtk_label_new("Appearance");
	GtkWidget *label_font = gtk_label_new("Font used:");

	// Buttons
	font_button = gtk_button_new_with_label("Choose Font");

	gtk_widget_set_hexpand(font_button, FALSE);			 // Don’t expand horizontally
	gtk_widget_set_halign(font_button, GTK_ALIGN_START); // or CENTER
	gtk_widget_set_vexpand(font_button, FALSE);			 // optional, for vertical

	// Box
	GtkWidget *fbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_box_append(GTK_BOX(fbox), label_font);
	gtk_box_append(GTK_BOX(fbox), font_button);

	// Connects with signals
	g_signal_connect(checkbox_wrap, "toggled", G_CALLBACK(on_checkboxes_toggled), "wrap");
	g_signal_connect(checkbox_whitespace, "toggled", G_CALLBACK(on_checkboxes_toggled),
					 "whitespace");
	g_signal_connect(checkbox_linenums, "toggled", G_CALLBACK(on_checkboxes_toggled), "linenums");
	g_signal_connect(checkbox_darkmode, "toggled", G_CALLBACK(on_checkboxes_toggled), "darkmode");

	g_signal_connect(font_button, "clicked", G_CALLBACK(on_choose_font_clicked), NULL);

	// Style labels
	gtk_widget_add_css_class(label_editing, "heading");
	gtk_label_set_xalign(GTK_LABEL(label_editing), 0.0);

	gtk_widget_add_css_class(label_appearance, "heading");
	gtk_label_set_xalign(GTK_LABEL(label_appearance), 0.0);

	// Adds everything to box
	gtk_box_append(GTK_BOX(main_box), label_editing);
	gtk_box_append(GTK_BOX(main_box), checkbox_wrap);
	gtk_box_append(GTK_BOX(main_box), checkbox_whitespace);
	gtk_box_append(GTK_BOX(main_box), checkbox_linenums);
	gtk_box_append(GTK_BOX(main_box), label_appearance);
	gtk_box_append(GTK_BOX(main_box), checkbox_darkmode);
	gtk_box_append(GTK_BOX(main_box), fbox);

	// Toggles checkboxes based on settings
	settings_state s = settings_get();
	updating_checkboxes_through_code = 1;
	gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox_darkmode), s.darkmode);
	gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox_linenums), s.linenums);
	gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox_wrap), s.textwrap);
	gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox_whitespace), s.whitespace);
	updating_checkboxes_through_code = 0;
	// Write font based on settings
	gtk_button_set_label(GTK_BUTTON(font_button), s.font);

	gtk_window_present(GTK_WINDOW(settings_window));
	log_info(__FILE__, "Created settings window");
}
