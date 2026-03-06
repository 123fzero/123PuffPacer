#pragma once

#include "../puff_pacer_settings.h"
#include <gui/view.h>

View* puff_about_view_alloc(void);
void puff_about_view_free(View* view);
void puff_about_view_reset(View* view);
void puff_about_view_set_language(View* view, PuffPacerLanguage language);
