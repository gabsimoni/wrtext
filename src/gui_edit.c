
#include "gui_edit.h"
#include "editor_file.h"
#include "gui_edit_menu.h"
#include "gui_settings.h"
#include "logger.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

/*!
	@brief Structure defining a list of files and mapping each one to a page number
*/
typedef struct {
	editor_file **files;
	editor_file_id *ids;
	unsigned long length;
	GtkWidget **page_widget;
} editor_file_list;

/*!
	@brief Container of all the files currently opened in the editor
*/
editor_file_list file_list;

/*!
	@brief GTK widget that holds all of the opened files
*/
GtkWidget *notebook;

/*!
	@brief Statusbar
*/
GtkWidget *statusbar;

/*!
	@brief Vertical box holding the notebook and statusbar
*/
GtkWidget *vbox;

/*!
	@brief Updates statusbar text with the data from the file passed as a parameter
	@param f Pointer to file
*/
void
statusbar_update(editor_file *f)
{

	char stat[64] = "";
	char lines_str[32];

	strcpy(stat, "  Filename: ");
	strcat(stat, f->file_name);
	strcat(stat, "  -  Lines: ");
	sprintf(lines_str, "%ld", f->lines);
	strcat(stat, lines_str);

	gtk_label_set_text(GTK_LABEL(statusbar), stat);
}

/*!
	@brief Function called whenever the user selects another opened file
*/
static void
notebook_on_switchpage(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data)
{
	// Find editor file from page widget. Iterate through all files
	for(int i = 0; i < file_list.length; i++) {
		if(file_list.page_widget[i] == page) {
			statusbar_update(file_list.files[i]);
			return;
		}
	}
}

/*!
	@brief Function called whenever the user adds text to a text-area. Updates file info and
   statusbar
*/
static void
textarea_on_insert_text(GtkTextBuffer *buffer, GtkTextIter *location, const char *text, int len,
						gpointer user_data)
{
	editor_file *f = (editor_file *)user_data;
	unsigned long lines_added = 0;

	// Counts lines added
	for(unsigned long i = 0; i < len; i++) {
		if(text[i] == '\n')
			lines_added++;
	}
	f->lines = f->lines + lines_added;

	statusbar_update(f);
}

/*!
	@brief Function called whenever the user adds text to a text-area. Updates file info and
   statusbar
*/
static void
textarea_on_delete_range(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end,
						 gpointer user_data)
{
	editor_file *f = (editor_file *)user_data;
	unsigned long lines_removed = 0;

	// Counts lines removed
	GtkTextIter iter = *start;
	while(gtk_text_iter_compare(&iter, end) < 0) {
		gunichar ch = gtk_text_iter_get_char(&iter);

		if(ch == '\n') {
			lines_removed++;
		}

		gtk_text_iter_forward_char(&iter);
	}
	f->lines = f->lines - lines_removed;

	statusbar_update(f);
}

void
log_file_list()
{
	log_info(__FILE__, "FILE LIST");
	log_info(__FILE__, "Size: %lu", file_list.length);

	for(int i = 0; i < file_list.length; i++) {
		log_info(__FILE__, "\tfile index: %d", i);
		log_info(__FILE__, "\tfile: %p", file_list.files[i]);
		log_info(__FILE__, "\tid: %lu", file_list.ids[i]);
		log_info(__FILE__, "\t");
	}
}

GtkWidget *
gui_edit_init(GtkApplication *app)
{

	// Initialize opened files structure
	file_list.files = NULL;
	file_list.ids = NULL;
	file_list.page_widget = NULL;
	file_list.length = 0;

	// Initialize window and box
	GtkWidget *window;
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "WRText");
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
	gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window), TRUE);

	g_signal_connect(window, "destroy", G_CALLBACK(gui_edit_cleanup), NULL);

	// Initialize menubar
	gui_edit_menu_init(app);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_window_set_child(GTK_WINDOW(window), vbox);

	notebook = gtk_notebook_new();

	// Connects to switch page function
	g_signal_connect(notebook, "switch-page", G_CALLBACK(notebook_on_switchpage), NULL);

	// Create a group, needed for reordering
	gtk_notebook_set_group_name(GTK_NOTEBOOK(notebook), "notebookgroup");
	// The notebook cannot have empty pages, so we always keep a page.
	// When the user closes the last open file, the notebook is hidden but the page isnt removed.
	// Tha page is removed when the user adds a new file again
	GtkWidget *dummy_label = gtk_label_new("dummy label");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), dummy_label, NULL);
	gtk_widget_hide(vbox);

	gtk_box_append(GTK_BOX(vbox), notebook);

	// Creates status bar
	statusbar = gtk_label_new("empty");

	// Expand + align
	gtk_widget_set_vexpand(statusbar, FALSE);
	gtk_widget_set_halign(statusbar, GTK_ALIGN_START);
	gtk_widget_set_valign(statusbar, GTK_ALIGN_END);

	// Adds status bar to the box
	gtk_box_append(GTK_BOX(vbox), statusbar);

	gtk_window_present(GTK_WINDOW(window));

	return window;
}

int
gui_edit_close_file(editor_file_id id)
{

	// Finds file from editor file id
	int file_index = -1;
	for(int i = 0; i < file_list.length; i++) {
		if(file_list.ids[i] == id) {
			file_index = i;
			break;
		}
	}

	if(file_index == -1) {
		log_err(__FILE__, "ID %lu not found in file list", id);
		return -1;
	}
	log_info(__FILE__, "ID %lu is at position %d", id, file_index);

	// Removes the page associated to that file

	int page_n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)); // get number of pages

	if(page_n > 1) {
		gtk_notebook_remove_page(
			(GtkNotebook *)notebook,
			gtk_notebook_page_num((GtkNotebook *)notebook, file_list.page_widget[file_index]));
	} else {
		// hide notebook instead
		gtk_widget_hide(vbox);
	}

	// Free file memory

	editor_file_delete(file_list.files[file_index]);

	if(file_list.length == 1) {
		free(file_list.files);
		free(file_list.page_widget);
		free(file_list.ids);
	} else {

		// Removes file from file list, this is done by shifting all files that come after by one.
		// It might seem slow but it's actually fast with a worst-case time of O(n)
		// Plus it allows for insertion with a time of O(1)

		// Shifts
		for(int i = file_index; i < file_list.length - 1; i++) {
			file_list.files[i] = file_list.files[i + 1];
			file_list.page_widget[i] = file_list.page_widget[i + 1];
			file_list.ids[i] = file_list.ids[i + 1];
		}

		// Frees memory
		file_list.files = realloc(file_list.files, sizeof(editor_file *) * (file_list.length - 1));
		file_list.page_widget
			= realloc(file_list.page_widget, sizeof(GtkWidget *) * (file_list.length - 1));
		file_list.ids = realloc(file_list.ids, sizeof(editor_file_id) * (file_list.length - 1));
	}
	file_list.length--;

	log_info(__FILE__, "closing file with ID \"%lu\"", id);
	log_file_list();

	return 0;
}

void
on_click_file_close(GtkButton *button, gpointer data)
{
	if(gui_edit_close_file(*(editor_file_id *)data) == -1) {
		// error
	}

	// This pointer was allocated at the time of opening the file
	free(data);
}

editor_file_id
gui_edit_add_file(editor_file *f)
{

	static editor_file_id last_assigned = 0;

	// If no file was open, allocate new memory for the array
	if(file_list.length == 0) {
		file_list.files = malloc(sizeof(editor_file *));
		file_list.page_widget = malloc(sizeof(GtkWidget *));
		file_list.ids = malloc(sizeof(editor_file_id));
	} else {
		file_list.files = realloc(file_list.files, sizeof(editor_file *) * (file_list.length + 1));
		file_list.page_widget
			= realloc(file_list.page_widget, sizeof(GtkWidget *) * (file_list.length + 1));
		file_list.ids = realloc(file_list.ids, sizeof(editor_file_id) * (file_list.length + 1));
	}

	file_list.files[file_list.length] = f; // Copy pointer to file added

	// Create new notebook page

	// Load file contents into a text area
	GtkTextBuffer *buff = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(buff, f->contents, f->size);

	// Creates scrollable window
	GtkWidget *scrolled_window = gtk_scrolled_window_new();

	// Create text area with file contents
	GtkWidget *text_area = gtk_text_view_new();

	// Make text area fill the whole space
	gtk_widget_set_hexpand(text_area, TRUE);
	gtk_widget_set_vexpand(text_area, TRUE);

	// Loads text buffer into textarea
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_area), buff);

	// Adds text area to scrollable window
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_area);

	// Set scrolled window policies
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC,
								   GTK_POLICY_ALWAYS);
	gtk_widget_set_hexpand(scrolled_window, TRUE);
	gtk_widget_set_vexpand(scrolled_window, TRUE);

	// Create widgets of the page title
	GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	GtkWidget *title_name = gtk_label_new(f->file_name);
	GtkWidget *close_image = gtk_image_new_from_icon_name("window-close");

	// Try to make them expand
	gtk_widget_set_hexpand(title_box, TRUE);

	GtkWidget *title_close = gtk_button_new();
	gtk_button_set_child(GTK_BUTTON(title_close), close_image);

	// Put them all in one box
	gtk_box_append(GTK_BOX(title_box), title_name);
	gtk_box_append(GTK_BOX(title_box), title_close);

	// Create page with the widgets
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_window, title_box);
	file_list.page_widget[file_list.length] = scrolled_window;

	int page_n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)); // get number of pages
	if(page_n == 2 && gtk_widget_get_visible(vbox) == false) {
		// Remove extra page and show
		gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), 0);
		statusbar_update(file_list.files[0]); // Has to update status bar with new file
		gtk_widget_show(vbox);
	}

	// Makes page reordable
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), scrolled_window, TRUE);

	// Connect function to the button that closes the file
	editor_file_id *fid = malloc(sizeof(editor_file_id));

	// Assigns new file id
	last_assigned++;
	*fid = last_assigned;
	file_list.ids[file_list.length] = *fid;

	// Connects close button to the closefile function
	g_signal_connect(title_close, "clicked", G_CALLBACK(on_click_file_close), fid);

	// Connects textarea actions to statusbar update functions
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_area));

	g_signal_connect(buffer, "insert-text", G_CALLBACK(textarea_on_insert_text), f);
	g_signal_connect(buffer, "delete-range", G_CALLBACK(textarea_on_delete_range), f);

	// Show new widgets
	gtk_widget_show(title_box);
	gtk_widget_show(title_name);
	gtk_widget_show(title_close);
	gtk_widget_show(text_area);

	file_list.length++; // Increment size counter of the list

	log_info(__FILE__, "added file \"%lu\"", *fid);
	log_file_list();

	return *fid;
}

void
gui_edit_cleanup()
{
}

void
gui_edit_add_random_file(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	static int called = 0;
	// TEST
	editor_file *fa = malloc(sizeof(editor_file));
	fa->file_name = calloc(1, 30);
	fa->file_path = malloc(5);
	strcpy(fa->file_name, "test0.txt");
	fa->file_name[4] = '0' + (char)called;

	fa->contents = malloc(3);
	fa->size = 3;
	fa->lines = 1;

	fa->contents[0] = '0' + (char)called;
	fa->contents[1] = '0' + (char)called;
	fa->contents[2] = '0' + (char)called;

	gui_edit_add_file(fa);

	called++;
}