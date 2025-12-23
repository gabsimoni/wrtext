/*
 * Simple file dialog helpers using GtkFileChooserDialog
 */

#pragma once
#include <gtk/gtk.h>

/**
 * @brief Show a GTK file dialog to open a file.
 *
 * Present "Open File" dialog with Cancel and Open buttons.
 * If the user accepts, the returned string is a newly allocated copy
 * Returns NULL if the dialog was cancelled.
 *
 * @param parent Parent window for the dialog.
 * @return char* Newly allocated path string or NULL.
 */
char *file_dialog_open_file(GtkWindow *parent);

/**
 * @brief Show a GTK file chooser dialog to save a file.
 *
 * Presents a modal "Save File" dialog with overwrite confirmation.
 * The `suggested_name` can be provided to pre-fill the name field.
 * If the user accepts, the returned string is a newly allocated copy
 * (use g_free() to free it). Returns NULL if the dialog was cancelled.
 *
 * @param parent Parent window for the dialog.
 * @param suggested_name Suggested filename to pre-fill.
 * @return char* Newly allocated path string or NULL.
 */
char *file_dialog_save_file(GtkWindow *parent, const char *suggested_name);
