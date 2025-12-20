#ifndef GUI_SETTINGS_H
#define GUI_SETTINGS_H

#include <gtk/gtk.h>

typedef struct {
    gboolean text_wrapping;
    gboolean whitespace_rendering;
    gboolean line_numbers;
    gboolean dark_mode;
    char *font_desc; // e.g., "Sans 11"
} GuiSettingsState;

/*!
 * @brief Opens the Settings window (non-blocking). If it already exists, presents it.
 * @param parent Parent window (usually WRText main window).
 */
void gui_settings_open(GtkWindow *parent);

/*!
 * @brief Returns the current settings snapshot (owned by module; do not free fields).
 */
const GuiSettingsState *gui_settings_get_state(void);

#endif
