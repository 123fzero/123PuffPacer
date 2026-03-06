#pragma once

#include <gui/view.h>
#include "../puff_pacer_settings.h"

typedef struct {
    PuffPacerStats stats;
    uint8_t screen;
    uint8_t log_offset;
    uint8_t day_index;
    uint32_t hourly_date_key;
} PuffStatsModel;

View* puff_stats_view_alloc(void);
void puff_stats_view_free(View* view);
