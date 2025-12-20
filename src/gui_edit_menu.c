#include "gui_edit_menu.h"
#include "gui_about.h"
#include "gui_edit.h"
#include "gui_settings.h"
#include <gtk/gtk.h>

static void
on_settings_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    GtkWindow *parent = GTK_WINDOW(user_data);
    gui_settings_open(parent);
}

static void
on_randfile_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* Reuse your existing test function */
    gui_edit_add_random_file(NULL, NULL, NULL);
}

GtkWidget *
gui_edit_menu_init(GtkApplication *app, GtkWindow *parent_window)
{
    (void)app;

    /*
     * This is an in-window top bar.
     * It avoids gtk_application_set_menubar(), so the user sees buttons inside the app.
     */
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_top(bar, 6);
    gtk_widget_set_margin_bottom(bar, 6);
    gtk_widget_set_margin_start(bar, 10);
    gtk_widget_set_margin_end(bar, 10);

    /* Left hamburger button (visual only for now) */
    GtkWidget *btn_menu = gtk_button_new_from_icon_name("open-menu-symbolic");
    gtk_widget_set_tooltip_text(btn_menu, "Menu");
    gtk_box_append(GTK_BOX(bar), btn_menu);

    /* Spacer so buttons look like a small bar */
    GtkWidget *spacer = gtk_label_new(NULL);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(bar), spacer);

    /* Buttons that MUST appear inside the window (your requirement) */
    GtkWidget *btn_rand = gtk_button_new_with_label("FileRand");
    g_signal_connect(btn_rand, "clicked", G_CALLBACK(on_randfile_clicked), NULL);
    gtk_box_append(GTK_BOX(bar), btn_rand);

    GtkWidget *btn_settings = gtk_button_new_with_label("Settings");
    g_signal_connect(btn_settings, "clicked", G_CALLBACK(on_settings_clicked), parent_window);
    gtk_box_append(GTK_BOX(bar), btn_settings);

    return bar;
}
