/**
 * @file file_dialog.c
 * @brief Implementations for GTK file dialogs.
 */

#include "file_dialog.h"
#include <gio/gio.h>
#include <stdlib.h>

/* Internal helper for waiting for the native dialog response.(sychrone) */
struct fd_cbdata {
	GMainLoop *loop; // block until reponse
	char *result;	 // path
};

static void
fd_on_response(GtkNativeDialog *native, gint response, gpointer user_data)
{
	struct fd_cbdata *d = user_data;

	if(response == GTK_RESPONSE_ACCEPT) {
		GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(native));
		if(file) {
			char *path = g_file_get_path(file);
			if(path)
				d->result = g_strdup(path);
			g_object_unref(file);
		}
	}

	if(d->loop)
		g_main_loop_quit(d->loop);
}

/**
 * @brief Show a blocking "Open File" dialog and return the chosen path.
 *
 * The returned string must be freed with g_free() by the caller.
 */
char *
file_dialog_open_file(GtkWindow *parent)
{
	GtkFileChooserNative *native = gtk_file_chooser_native_new(
		"Open File", parent, GTK_FILE_CHOOSER_ACTION_OPEN, "_Open", "_Cancel");

	struct fd_cbdata *cb = g_new0(struct fd_cbdata, 1);
	cb->loop = g_main_loop_new(NULL, FALSE);
	cb->result = NULL;

	g_signal_connect(native, "response", G_CALLBACK(fd_on_response), cb);
	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));
	g_main_loop_run(cb->loop);

	char *result = cb->result;
	g_main_loop_unref(cb->loop);
	g_free(cb);
	g_object_unref(native);
	return result;
}

/**
 * @brief Show a blocking "Save File" dialog and return the chosen path.
 *
 * The returned string must be freed with g_free() by the caller.
 */
char *
file_dialog_save_file(GtkWindow *parent, const char *suggested_name)
{
	GtkFileChooserNative *native = gtk_file_chooser_native_new(
		"Save File", parent, GTK_FILE_CHOOSER_ACTION_SAVE, "_Save", "_Cancel");

	if(suggested_name)
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(native), suggested_name);

	struct fd_cbdata *cb = g_new0(struct fd_cbdata, 1);
	cb->loop = g_main_loop_new(NULL, FALSE);
	cb->result = NULL;

	g_signal_connect(native, "response", G_CALLBACK(fd_on_response), cb);
	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));
	g_main_loop_run(cb->loop);

	char *result = cb->result;
	g_main_loop_unref(cb->loop);
	g_free(cb);
	g_object_unref(native);
	return result;
}
