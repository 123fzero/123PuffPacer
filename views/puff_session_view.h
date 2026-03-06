#pragma once

#include "../puff_pacer_settings.h"
#include <gui/view.h>

typedef struct {
    uint32_t current_puff;
    uint32_t total_puffs;
    uint32_t seconds_remaining;
    uint32_t total_elapsed;
    bool paused;
    uint8_t smoke_timer;
    uint8_t smoke_frame;
    uint8_t language;
} PuffSessionModel;

View* puff_session_view_alloc(void);
void puff_session_view_free(View* view);
