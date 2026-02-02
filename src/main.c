#include "editor_file.h"
#include "gui_edit.h"
#include "gui_edit_menu.h"
#include "logger.h"
#include "string.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

int cmd_files;
char **cmd_paths;

/*!
 * @brief Function called once the application runs
 * @param app Pointer to the GTK application
 * @param user_data Other info
 */
static void
activate(GtkApplication *app, gpointer user_data)
{
	gui_edit_init(app, cmd_files, cmd_paths);
}

/*!
 * @brief Function called to read command line parameters
 * @param app Pointer to the GTK application
 * @param user_data Other info
 */
static void
on_command_line(GApplication *app, GApplicationCommandLine *cmdline, gpointer user_data)
{
	int argc;
	char **argv = g_application_command_line_get_arguments(cmdline, &argc);

	cmd_files = argc - 1;
	cmd_paths = calloc(sizeof(char *), cmd_files);

	// Loads parameters and tries to load them as files
	for(int i = 0; i < cmd_files; i++) {
		cmd_paths[i] = strdup(argv[i + 1]);
		log_info(__FILE__, "param %d: %s", i, cmd_paths[i]);
	}

	g_application_activate(app); // show main window
	g_application_command_line_set_exit_status(cmdline, 0);
}

/*!
 * @brief Main function of WRText. (Doxygen test)
 * @param argc Number of arguments passed to the program
 * @param argv List of arguments as strings
 */
int
main(int argc, char **argv)
{
	
#ifdef _WIN32
    g_setenv(
        "GSETTINGS_SCHEMA_DIR",
        "share\\glib-2.0\\schemas",
        TRUE
    );
#endif

	GtkApplication *app; // Variable that stores the pointer to the application object
	int status;			 // Variable used to save the result of the application

	// Creates the GTK application and puts it in the pointer
	app = gtk_application_new("org.gtk.wrtext", G_APPLICATION_HANDLES_COMMAND_LINE);

	// Parameters have to be parsed in a special function
	g_signal_connect(app, "command-line", G_CALLBACK(on_command_line), NULL);

	// GTK uses signal, here we make it so that once the "activate" signal is called, the
	// activate function is called.
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	// The application is run and this triggers the "activate" signal, calling the activate
	// function
	status = g_application_run(G_APPLICATION(app), argc, argv);

	// Just some memory cleanup
	g_object_unref(app);

	return status; // Returns result of execution
}
