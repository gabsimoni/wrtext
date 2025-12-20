#include "gui_settings.h"

#include <gtk/gtk.h>
#include <pango/pango.h>
#include <string.h>

/*
 * Simple in-module state.
 * Later you can wire "Apply" to actually change editor widgets.
 */
static GuiSettingsState g_state = {
    .text_wrapping = FALSE,
    .whitespace_rendering = FALSE,
    .line_numbers = FALSE,
    .dark_mode = FALSE,
    .font_desc = NULL,
};

static GtkWindow *g_settings_window = NULL;

/* Widgets we need to read/write */
typedef struct {
    GtkCheckButton *cb_wrap;
    GtkCheckButton *cb_ws;
    GtkCheckButton *cb_ln;
    GtkCheckButton *cb_dark;
    GtkLabel *lbl_font_used;
    GtkFontDialogButton *btn_font;
    char *draft_font; /* temporary selection before Apply */
} UiRefs;

static void
ui_refs_free(gpointer data)
{
    UiRefs *ui = (UiRefs *)data;
    if (!ui) return;
    g_free(ui->draft_font);
    g_free(ui);
}

static void
update_font_label(UiRefs *ui, const char *font_desc)
{
    if (!font_desc || font_desc[0] == '\0') {
        gtk_label_set_text(ui->lbl_font_used, "Font used: Sans-serif");
        return;
    }

    char buf[256];
    g_snprintf(buf, sizeof(buf), "Font used: %s", font_desc);
    gtk_label_set_text(ui->lbl_font_used, buf);
}

static void
on_cancel_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    GtkWindow *win = GTK_WINDOW(user_data);
    gtk_window_close(win);
}

static void
on_apply_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    UiRefs *ui = (UiRefs *)user_data;

    /* Commit checkboxes */
    g_state.text_wrapping = gtk_check_button_get_active(ui->cb_wrap);
    g_state.whitespace_rendering = gtk_check_button_get_active(ui->cb_ws);
    g_state.line_numbers = gtk_check_button_get_active(ui->cb_ln);
    g_state.dark_mode = gtk_check_button_get_active(ui->cb_dark);

    /* Commit font (if picked) */
    if (ui->draft_font && ui->draft_font[0] != '\0') {
        g_free(g_state.font_desc);
        g_state.font_desc = g_strdup(ui->draft_font);
    }

    /* Close settings window */
    if (g_settings_window) {
        gtk_window_close(g_settings_window);
    }
}

static void
on_font_changed(GObject *obj, GParamSpec *pspec, gpointer user_data)
{
    (void)pspec;
    UiRefs *ui = (UiRefs *)user_data;

    /* GtkFontDialogButton property "font-desc" is a PangoFontDescription* (boxed type) */
    PangoFontDescription *desc = NULL;
    g_object_get(obj, "font-desc", &desc, NULL);

    if (desc) {
        char *s = pango_font_description_to_string(desc);

        g_free(ui->draft_font);
        ui->draft_font = g_strdup(s);

        update_font_label(ui, ui->draft_font);

        g_free(s);
        pango_font_description_free(desc);
    }
}

static GtkWidget *
section_title(const char *text)
{
    GtkWidget *lbl = gtk_label_new(text);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl, "title-3"); /* nice bold-ish in GTK */
    return lbl;
}

static GtkWidget *
build_settings_content(GtkWindow *parent)
{
    (void)parent;

    UiRefs *ui = g_new0(UiRefs, 1);
    /* Attach UiRefs to window; will be freed when window is finalized */
    g_object_set_data_full(G_OBJECT(g_settings_window), "ui-refs", ui, ui_refs_free);

    /* Outer container */
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_margin_top(outer, 16);
    gtk_widget_set_margin_bottom(outer, 16);
    gtk_widget_set_margin_start(outer, 18);
    gtk_widget_set_margin_end(outer, 18);

    /* --- Text editing --- */
    gtk_box_append(GTK_BOX(outer), section_title("Text editing"));

    ui->cb_wrap = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Text wrapping"));
    ui->cb_ws   = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Whitespace rendering"));
    ui->cb_ln   = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Line numbers"));

    gtk_check_button_set_active(ui->cb_wrap, g_state.text_wrapping);
    gtk_check_button_set_active(ui->cb_ws, g_state.whitespace_rendering);
    gtk_check_button_set_active(ui->cb_ln, g_state.line_numbers);

    gtk_box_append(GTK_BOX(outer), GTK_WIDGET(ui->cb_wrap));
    gtk_box_append(GTK_BOX(outer), GTK_WIDGET(ui->cb_ws));
    gtk_box_append(GTK_BOX(outer), GTK_WIDGET(ui->cb_ln));

    /* --- Appearance --- */
    gtk_box_append(GTK_BOX(outer), section_title("Appearance"));

    ui->cb_dark = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Dark mode"));
    gtk_check_button_set_active(ui->cb_dark, g_state.dark_mode);
    gtk_box_append(GTK_BOX(outer), GTK_WIDGET(ui->cb_dark));

    /* Font line: label + pick font button */
    GtkWidget *font_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(font_row, GTK_ALIGN_FILL);

    ui->lbl_font_used = GTK_LABEL(gtk_label_new(NULL));
    gtk_widget_set_halign(GTK_WIDGET(ui->lbl_font_used), GTK_ALIGN_START);
    gtk_widget_set_hexpand(GTK_WIDGET(ui->lbl_font_used), TRUE);

    /* Modern GTK4 font picker */
    GtkFontDialog *dialog = gtk_font_dialog_new();
    ui->btn_font = GTK_FONT_DIALOG_BUTTON(gtk_font_dialog_button_new(dialog));

    /* Use property instead of gtk_font_dialog_button_set_title (compat-safe) */
    g_object_set(G_OBJECT(ui->btn_font), "title", "Pick font", NULL);

    /* Preload current font if any */
    if (g_state.font_desc && g_state.font_desc[0] != '\0') {
        PangoFontDescription *d = pango_font_description_from_string(g_state.font_desc);
        g_object_set(G_OBJECT(ui->btn_font), "font-desc", d, NULL);
        pango_font_description_free(d);
        update_font_label(ui, g_state.font_desc);
    } else {
        update_font_label(ui, NULL);
    }

    /* Keep temporary draft until Apply */
    if (g_state.font_desc) ui->draft_font = g_strdup(g_state.font_desc);

    g_signal_connect(ui->btn_font, "notify::font-desc", G_CALLBACK(on_font_changed), ui);

    gtk_box_append(GTK_BOX(font_row), GTK_WIDGET(ui->lbl_font_used));
    gtk_box_append(GTK_BOX(font_row), GTK_WIDGET(ui->btn_font));
    gtk_box_append(GTK_BOX(outer), font_row);

    /* Bottom buttons: Cancel / Apply */
    GtkWidget *bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(bottom, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(bottom, 10);

    GtkWidget *btn_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *btn_apply  = gtk_button_new_with_label("Apply");

    gtk_widget_set_size_request(btn_cancel, 120, -1);
    gtk_widget_set_size_request(btn_apply, 120, -1);

    g_signal_connect(btn_cancel, "clicked", G_CALLBACK(on_cancel_clicked), g_settings_window);
    g_signal_connect(btn_apply, "clicked", G_CALLBACK(on_apply_clicked), ui);

    gtk_box_append(GTK_BOX(bottom), btn_cancel);
    gtk_box_append(GTK_BOX(bottom), btn_apply);
    gtk_box_append(GTK_BOX(outer), bottom);

    return outer;
}

/* C callback instead of C++ lambda */
static gboolean
on_settings_close_request(GtkWindow *win, gpointer user_data)
{
    (void)win;
    (void)user_data;

    /* Allow window to close; reset pointer so it can be opened again */
    g_settings_window = NULL;
    return FALSE;
}

void
gui_settings_open(GtkWindow *parent)
{
    if (g_settings_window) {
        gtk_window_present(g_settings_window);
        return;
    }

    g_settings_window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(g_settings_window, "Settings");
    gtk_window_set_default_size(g_settings_window, 310, 360);
    gtk_window_set_transient_for(g_settings_window, parent);
    gtk_window_set_modal(g_settings_window, TRUE);

    GtkWidget *content = build_settings_content(parent);
    gtk_window_set_child(g_settings_window, content);

    /* When user closes the window, reset pointer so it can be opened again */
    g_signal_connect(g_settings_window, "close-request",
                     G_CALLBACK(on_settings_close_request), NULL);

    gtk_window_present(g_settings_window);
}

const GuiSettingsState *
gui_settings_get_state(void)
{
    return &g_state;
}
