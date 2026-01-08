#include "settings.h"
#include <string.h>

// Actual application settings, safely encapsulated.
settings_state t;

settings_state
settings_get()
{
	return t;
}

void
settings_set(settings_state *s)
{
	t.darkmode = s->darkmode;
	t.font[0] = 0;
	strcpy(t.font, s->font);
	t.linenums = s->linenums;
	t.textwrap = s->textwrap;
	t.whitespace = s->whitespace;
}