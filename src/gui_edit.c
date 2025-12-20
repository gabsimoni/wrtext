#include "gui_edit.h"
#include "editor_file.h"
#include "gui_edit_menu.h"
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
static editor_file_list file_list;

/*!
    @brief GTK widget that holds all of the opened files
*/
static GtkWidget *notebook;

void
log_file_list()
{
    log_info(__FILE__, "FILE LIST");
    log_info(__FILE__, "Size: %lu", file_list.length);

    for(int i = 0; i < (int)file_list.length; i++) {
        log_info(__FILE__, "\tfile index: %d", i);
        log_info(__FILE__, "\tfile: %p", file_list.files[i]);
        log_info(__FILE__, "\tid: %lu", file_list.ids[i]);
        log_info(__FILE__, "\t");
    }
}

GtkWidget *
gui_edit_init(GtkApplication *app)
{
    /* Initialize opened files structure */
    file_list.files = NULL;
    file_list.ids = NULL;
    file_list.page_widget = NULL;
    file_list.length = 0;

    /* Initialize window */
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "WRText");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 650);

    /*
        IMPORTANT (macOS):
        If you enable the GTK menubar, it appears in the *system top menu*,
        not inside the application window. You explicitly want an in-window bar,
        so keep menubar OFF.
    */
    gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window), FALSE);

    g_signal_connect(window, "destroy", G_CALLBACK(gui_edit_cleanup), NULL);

    /* Main vertical container */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    /*
        Create the in-window top bar.
        FIX: parameter order must match: (GtkApplication*, GtkWindow*)
    */
    GtkWidget *topbar = gui_edit_menu_init(app, GTK_WINDOW(window));
    if(topbar != NULL) {
        gtk_box_append(GTK_BOX(vbox), topbar);
    } else {
        log_err(__FILE__, "gui_edit_menu_init returned NULL (topbar not created)");
    }

    /* Notebook that holds opened files */
    notebook = gtk_notebook_new();

    /* Needed for drag-reordering tabs */
    gtk_notebook_set_group_name(GTK_NOTEBOOK(notebook), "notebookgroup");

    /* Make notebook take the remaining space */
    gtk_widget_set_vexpand(notebook, TRUE);
    gtk_widget_set_hexpand(notebook, TRUE);

    gtk_box_append(GTK_BOX(vbox), notebook);

    gtk_window_present(GTK_WINDOW(window));
    return window;
}

int
gui_edit_close_file(editor_file_id id)
{
    /* Find file from editor file id */
    int file_index = -1;
    for(int i = 0; i < (int)file_list.length; i++) {
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

    /* Remove page associated to that file */
    gtk_notebook_remove_page(
        GTK_NOTEBOOK(notebook),
        gtk_notebook_page_num(GTK_NOTEBOOK(notebook), file_list.page_widget[file_index])
    );

    /* Free file memory */
    editor_file_delete(file_list.files[file_index]);

    if(file_list.length == 1) {
        free(file_list.files);
        free(file_list.page_widget);
        free(file_list.ids);
        file_list.files = NULL;
        file_list.page_widget = NULL;
        file_list.ids = NULL;
    } else {
        /* Shift everything after file_index back by one */
        for(int i = file_index; i < (int)file_list.length - 1; i++) {
            file_list.files[i] = file_list.files[i + 1];
            file_list.page_widget[i] = file_list.page_widget[i + 1];
            file_list.ids[i] = file_list.ids[i + 1];
        }

        /* Shrink arrays */
        file_list.files = realloc(file_list.files, sizeof(editor_file *) * (file_list.length - 1));
        file_list.page_widget =
            realloc(file_list.page_widget, sizeof(GtkWidget *) * (file_list.length - 1));
        file_list.ids = realloc(file_list.ids, sizeof(editor_file_id) * (file_list.length - 1));
    }

    file_list.length--;

    log_info(__FILE__, "closing file \"%lu\"", id);
    log_file_list();

    return 0;
}

void
on_click_file_close(GtkButton *button, gpointer data)
{
    (void)button;

    if(gui_edit_close_file(*(editor_file_id *)data) == -1) {
        /* error handling could go here */
    }

    /* This pointer was allocated at the time of opening the file */
    free(data);
}

editor_file_id
gui_edit_add_file(editor_file *f)
{
    static editor_file_id last_assigned = 0;

    /* Allocate or grow arrays */
    if(file_list.length == 0) {
        file_list.files = malloc(sizeof(editor_file *));
        file_list.page_widget = malloc(sizeof(GtkWidget *));
        file_list.ids = malloc(sizeof(editor_file_id));
    } else {
        file_list.files = realloc(file_list.files, sizeof(editor_file *) * (file_list.length + 1));
        file_list.page_widget =
            realloc(file_list.page_widget, sizeof(GtkWidget *) * (file_list.length + 1));
        file_list.ids = realloc(file_list.ids, sizeof(editor_file_id) * (file_list.length + 1));
    }

    file_list.files[file_list.length] = f; /* store pointer */

    /* Load file contents into a text buffer */
    GtkTextBuffer *buff = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(buff, f->contents, f->size);

    /* Create text area */
    GtkWidget *text_area = gtk_text_view_new();
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_area), buff);

    /* Create page tab title: [filename] [x] */
    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *title_name = gtk_label_new(f->file_name);

    GtkWidget *close_image = gtk_image_new_from_icon_name("window-close");
    GtkWidget *title_close = gtk_button_new();
    gtk_button_set_child(GTK_BUTTON(title_close), close_image);

    gtk_box_append(GTK_BOX(title_box), title_name);
    gtk_box_append(GTK_BOX(title_box), title_close);

    /* Append page */
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), text_area, title_box);
    file_list.page_widget[file_list.length] = text_area;

    /* Allow reordering tabs */
    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), text_area, TRUE);

    /* Assign new file id */
    editor_file_id *fid = malloc(sizeof(editor_file_id));
    last_assigned++;
    *fid = last_assigned;
    file_list.ids[file_list.length] = *fid;

    g_signal_connect(title_close, "clicked", G_CALLBACK(on_click_file_close), fid);

    /*
        GTK4: gtk_widget_show is deprecated (4.10+). Widgets are visible by default
        after being added to a visible container, but we can explicitly set visible.
    */
    gtk_widget_set_visible(title_box, TRUE);
    gtk_widget_set_visible(title_name, TRUE);
    gtk_widget_set_visible(title_close, TRUE);
    gtk_widget_set_visible(text_area, TRUE);

    file_list.length++;

    log_info(__FILE__, "added file \"%lu\"", *fid);
    log_file_list();

    return *fid;
}

void
gui_edit_cleanup()
{
    if(file_list.files != NULL)
        free(file_list.files);
    if(file_list.page_widget != NULL)
        free(file_list.page_widget);
    if(file_list.ids != NULL)
        free(file_list.ids);

    file_list.files = NULL;
    file_list.page_widget = NULL;
    file_list.ids = NULL;
    file_list.length = 0;
}

void
gui_edit_add_random_file(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;
    (void)parameter;
    (void)user_data;

    static int called = 0;

    /* TEST: create a fake file */
    editor_file *fa = malloc(sizeof(editor_file));
    fa->file_name = malloc(30);
    strcpy(fa->file_name, "test0.txt");
    fa->file_name[4] = '0' + (char)called;

    fa->contents = malloc(3);
    fa->size = 3;

    fa->contents[0] = '0' + (char)called;
    fa->contents[1] = '0' + (char)called;
    fa->contents[2] = '0' + (char)called;

    gui_edit_add_file(fa);
    called++;
}
