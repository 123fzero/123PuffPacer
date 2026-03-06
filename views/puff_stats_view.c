#include "puff_stats_view.h"
#include "../puff_pacer_app.h"
#include <datetime/datetime.h>
#include <furi_hal_rtc.h>
#include <gui/elements.h>
#include <stdio.h>

#define PUFF_STATS_VISIBLE_LOGS 3
#define PUFF_STATS_GRAPH_DAYS   7
#define PUFF_STATS_SCREEN_LIST  0
#define PUFF_STATS_SCREEN_DAILY 1
#define PUFF_STATS_SCREEN_HOURLY 2

static uint32_t puff_stats_date_key(uint16_t year, uint8_t month, uint8_t day) {
    return ((uint32_t)year * 10000U) + ((uint32_t)month * 100U) + day;
}

static bool puff_stats_is_leap_year(uint16_t year) {
    return ((year % 4U) == 0U) && (((year % 100U) != 0U) || ((year % 400U) == 0U));
}

static uint8_t puff_stats_days_in_month(uint16_t year, uint8_t month) {
    static const uint8_t month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(month == 2 && puff_stats_is_leap_year(year)) {
        return 29;
    }

    return month_days[month - 1];
}

static void puff_stats_step_back_one_day(DateTime* dt) {
    if(dt->day > 1) {
        dt->day--;
        return;
    }

    if(dt->month > 1) {
        dt->month--;
    } else {
        dt->month = 12;
        dt->year--;
    }

    dt->day = puff_stats_days_in_month(dt->year, dt->month);
}

static uint8_t puff_stats_count_for_day(const PuffPacerStats* stats, uint32_t date_key) {
    uint8_t count = 0;

    for(uint32_t i = 0; i < stats->log_count; i++) {
        if(stats->logs[i].date_key == date_key) {
            count++;
        }
    }

    return count;
}

static uint8_t puff_stats_count_for_hour(const PuffPacerStats* stats, uint32_t date_key, uint8_t hour) {
    uint8_t count = 0;

    for(uint32_t i = 0; i < stats->log_count; i++) {
        const PuffPacerStatsEntry* entry = &stats->logs[i];
        if(entry->date_key != date_key) continue;

        uint32_t entry_hour = entry->time_key / 100U;
        uint32_t entry_minute = entry->time_key % 100U;
        if(entry_hour < 24U && entry_minute < 60U && entry_hour == hour) {
            count++;
        }
    }

    return count;
}

static uint32_t puff_stats_date_key_for_day_index(uint8_t day_index) {
    if(day_index >= PUFF_STATS_GRAPH_DAYS) {
        day_index = PUFF_STATS_GRAPH_DAYS - 1;
    }

    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);

    uint8_t steps_back = (PUFF_STATS_GRAPH_DAYS - 1) - day_index;
    while(steps_back > 0) {
        puff_stats_step_back_one_day(&dt);
        steps_back--;
    }

    return puff_stats_date_key(dt.year, dt.month, dt.day);
}

static bool puff_stats_get_recent_entry(
    const PuffPacerStats* stats,
    uint8_t offset,
    PuffPacerStatsEntry* entry) {
    if(offset >= stats->log_count) {
        return false;
    }

    int32_t index = (int32_t)stats->next_index - 1 - offset;
    while(index < 0) {
        index += PUFF_PACER_STATS_MAX_LOGS;
    }

    *entry = stats->logs[index];
    return entry->date_key != 0;
}

static void puff_stats_view_draw_list(Canvas* canvas, const PuffStatsModel* model) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, "Statistics");

    char count_buf[24];
    snprintf(count_buf, sizeof(count_buf), "S:%lu D:%lu", (unsigned long)model->stats.started_count, (unsigned long)model->stats.completed_count);
    canvas_draw_str_aligned(canvas, 126, 10, AlignRight, AlignBottom, count_buf);

    canvas_set_font(canvas, FontPrimary);
    if(model->stats.log_count == 0) {
        canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignBottom, "No completed");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignBottom, "Finish one to log");
    } else {
        char header_buf[24];
        uint8_t shown_from = model->log_offset + 1;
        uint8_t shown_to = model->log_offset + PUFF_STATS_VISIBLE_LOGS;
        if(shown_to > model->stats.log_count) {
            shown_to = model->stats.log_count;
        }
        snprintf(
            header_buf,
            sizeof(header_buf),
            "Log %u-%u/%lu",
            shown_from,
            shown_to,
            (unsigned long)model->stats.log_count);
        canvas_draw_str(canvas, 2, 22, header_buf);

        canvas_set_font(canvas, FontSecondary);
        for(uint8_t row = 0; row < PUFF_STATS_VISIBLE_LOGS; row++) {
            PuffPacerStatsEntry entry;
            if(!puff_stats_get_recent_entry(&model->stats, model->log_offset + row, &entry)) {
                break;
            }

            uint32_t month = (entry.date_key / 100U) % 100U;
            uint32_t day = entry.date_key % 100U;
            uint32_t hour = entry.time_key / 100U;
            uint32_t minute = entry.time_key % 100U;
            char line_buf[32];
            if(entry.time_key > 0 && hour < 24 && minute < 60) {
                snprintf(
                    line_buf,
                    sizeof(line_buf),
                    "%02lu.%02lu %02lu:%02lu %lux%lu",
                    (unsigned long)day,
                    (unsigned long)month,
                    (unsigned long)hour,
                    (unsigned long)minute,
                    (unsigned long)entry.puff_count,
                    (unsigned long)entry.interval_sec);
            } else {
                snprintf(
                    line_buf,
                    sizeof(line_buf),
                    "%02lu.%02lu --:-- %lux%lu",
                    (unsigned long)day,
                    (unsigned long)month,
                    (unsigned long)entry.puff_count,
                    (unsigned long)entry.interval_sec);
            }
            canvas_draw_str(canvas, 2, 32 + (row * 10), line_buf);
        }
    }

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 62, "OK Graph");
    if(model->stats.log_count > PUFF_STATS_VISIBLE_LOGS) {
        canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, "^v Scroll");
    }
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, "< Back");
}

static void puff_stats_view_draw_daily_graph(Canvas* canvas, const PuffStatsModel* model) {
    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);

    uint8_t counts[PUFF_STATS_GRAPH_DAYS] = {0};
    uint8_t labels[PUFF_STATS_GRAPH_DAYS] = {0};
    uint8_t max_count = 1;

    for(int8_t i = PUFF_STATS_GRAPH_DAYS - 1; i >= 0; i--) {
        uint32_t date_key = puff_stats_date_key(dt.year, dt.month, dt.day);
        counts[i] = puff_stats_count_for_day(&model->stats, date_key);
        labels[i] = dt.day;
        if(counts[i] > max_count) {
            max_count = counts[i];
        }
        if(i > 0) {
            puff_stats_step_back_one_day(&dt);
        }
    }

    const uint8_t chart_x = 4;
    const uint8_t chart_y = 10;
    const uint8_t chart_w = 120;
    const uint8_t chart_h = 30;
    const uint8_t bar_w = 12;
    const uint8_t gap = 5;
    elements_frame(canvas, chart_x, chart_y, chart_w, chart_h);

    for(uint8_t i = 0; i < PUFF_STATS_GRAPH_DAYS; i++) {
        uint8_t bar_height = 0;
        if(counts[i] > 0) {
            bar_height = (uint8_t)((counts[i] * (chart_h - 4)) / max_count);
            if(bar_height == 0) {
                bar_height = 1;
            }
        }

        uint8_t x = chart_x + 3 + (i * (bar_w + gap));
        char count_buf[4];
        snprintf(count_buf, sizeof(count_buf), "%u", counts[i]);
        canvas_draw_str_aligned(
            canvas, x + (bar_w / 2), 8, AlignCenter, AlignBottom, count_buf);

        if(bar_height > 0) {
            canvas_draw_box(canvas, x, chart_y + chart_h - 2 - bar_height, bar_w, bar_height);
        }

        if(i == model->day_index) {
            elements_frame(canvas, x - 1, chart_y + 1, bar_w + 2, chart_h - 2);
        }

        char label_buf[4];
        snprintf(label_buf, sizeof(label_buf), "%u", labels[i]);
        canvas_draw_str_aligned(
            canvas, x + (bar_w / 2), 51, AlignCenter, AlignBottom, label_buf);
    }

    char total_buf[24];
    snprintf(
        total_buf,
        sizeof(total_buf),
        "D:%lu",
        (unsigned long)model->stats.completed_count);
    canvas_draw_str(canvas, 2, 62, "OK Hours");
    canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, "Daily graph");
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, "< List");
    canvas_draw_str_aligned(canvas, 126, 10, AlignRight, AlignBottom, total_buf);
}

static void puff_stats_view_draw_hourly_graph(Canvas* canvas, const PuffStatsModel* model) {
    uint8_t counts[24] = {0};
    uint8_t max_count = 1;
    uint16_t total_count = 0;

    for(uint8_t hour = 0; hour < 24; hour++) {
        counts[hour] = puff_stats_count_for_hour(&model->stats, model->hourly_date_key, hour);
        total_count += counts[hour];
        if(counts[hour] > max_count) {
            max_count = counts[hour];
        }
    }

    uint32_t month = (model->hourly_date_key / 100U) % 100U;
    uint32_t day = model->hourly_date_key % 100U;
    char title_buf[24];
    snprintf(title_buf, sizeof(title_buf), "%02lu.%02lu by hour", (unsigned long)day, (unsigned long)month);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignBottom, title_buf);

    const uint8_t chart_x = 14;
    const uint8_t chart_y = 12;
    const uint8_t chart_w = 100;
    const uint8_t chart_h = 30;
    const uint8_t bar_w = 3;
    const uint8_t gap = 1;
    elements_frame(canvas, chart_x, chart_y, chart_w, chart_h);

    for(uint8_t hour = 0; hour < 24; hour++) {
        uint8_t bar_height = 0;
        if(counts[hour] > 0) {
            bar_height = (uint8_t)((counts[hour] * (chart_h - 4)) / max_count);
            if(bar_height == 0) {
                bar_height = 1;
            }
        }

        uint8_t x = chart_x + 2 + (hour * (bar_w + gap));
        if(bar_height > 0) {
            canvas_draw_box(canvas, x, chart_y + chart_h - 2 - bar_height, bar_w, bar_height);
        }
    }

    canvas_draw_str_aligned(canvas, chart_x + 2, 50, AlignCenter, AlignBottom, "0");
    canvas_draw_str_aligned(canvas, chart_x + 2 + 6 * (bar_w + gap), 50, AlignCenter, AlignBottom, "6");
    canvas_draw_str_aligned(canvas, chart_x + 2 + 12 * (bar_w + gap), 50, AlignCenter, AlignBottom, "12");
    canvas_draw_str_aligned(canvas, chart_x + 2 + 18 * (bar_w + gap), 50, AlignCenter, AlignBottom, "18");
    canvas_draw_str_aligned(canvas, chart_x + 2 + 23 * (bar_w + gap), 50, AlignCenter, AlignBottom, "23");

    char total_buf[16];
    snprintf(total_buf, sizeof(total_buf), "Count:%u", total_count);
    canvas_draw_str(canvas, 2, 62, "< Days");
    canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, "Hourly");
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, total_buf);
}

static void puff_stats_view_draw(Canvas* canvas, void* model) {
    const PuffStatsModel* m = model;
    canvas_clear(canvas);

    if(m->screen == PUFF_STATS_SCREEN_LIST) {
        puff_stats_view_draw_list(canvas, m);
    } else if(m->screen == PUFF_STATS_SCREEN_DAILY) {
        puff_stats_view_draw_daily_graph(canvas, m);
    } else {
        puff_stats_view_draw_hourly_graph(canvas, m);
    }
}

static bool puff_stats_view_input(InputEvent* event, void* context) {
    PuffPacerApp* app = context;

    if(event->type != InputTypeShort) {
        return false;
    }

    if(event->key == InputKeyOk) {
        bool handled = false;
        with_view_model(
            app->stats_view,
            PuffStatsModel * m,
            {
                if(m->screen == PUFF_STATS_SCREEN_LIST) {
                    m->screen = PUFF_STATS_SCREEN_DAILY;
                    handled = true;
                } else if(m->screen == PUFF_STATS_SCREEN_DAILY) {
                    m->hourly_date_key = puff_stats_date_key_for_day_index(m->day_index);
                    m->screen = PUFF_STATS_SCREEN_HOURLY;
                    handled = true;
                }
            },
            true);
        return handled;
    }

    if(event->key == InputKeyBack) {
        bool handled = false;
        with_view_model(
            app->stats_view,
            PuffStatsModel * m,
            {
                if(m->screen == PUFF_STATS_SCREEN_HOURLY) {
                    m->screen = PUFF_STATS_SCREEN_DAILY;
                    handled = true;
                } else if(m->screen == PUFF_STATS_SCREEN_DAILY) {
                    m->screen = PUFF_STATS_SCREEN_LIST;
                    handled = true;
                }
            },
            true);
        return handled;
    }

    if(event->key == InputKeyUp || event->key == InputKeyDown) {
        bool handled = false;
        with_view_model(
            app->stats_view,
            PuffStatsModel * m,
            {
                if(m->screen == PUFF_STATS_SCREEN_LIST && m->stats.log_count > PUFF_STATS_VISIBLE_LOGS) {
                    uint8_t max_offset = m->stats.log_count - PUFF_STATS_VISIBLE_LOGS;
                    if(event->key == InputKeyUp && m->log_offset > 0) {
                        m->log_offset--;
                        handled = true;
                    } else if(event->key == InputKeyDown && m->log_offset < max_offset) {
                        m->log_offset++;
                        handled = true;
                    }
                }
            },
            true);
        return handled;
    }

    if(event->key == InputKeyLeft || event->key == InputKeyRight) {
        bool handled = false;
        with_view_model(
            app->stats_view,
            PuffStatsModel * m,
            {
                if(m->screen == PUFF_STATS_SCREEN_DAILY) {
                    if(event->key == InputKeyLeft && m->day_index > 0) {
                        m->day_index--;
                        handled = true;
                    } else if(event->key == InputKeyRight &&
                              m->day_index + 1 < PUFF_STATS_GRAPH_DAYS) {
                        m->day_index++;
                        handled = true;
                    }
                }
            },
            true);
        return handled;
    }

    return false;
}

View* puff_stats_view_alloc(void) {
    View* view = view_alloc();
    view_allocate_model(view, ViewModelTypeLocking, sizeof(PuffStatsModel));
    view_set_draw_callback(view, puff_stats_view_draw);
    view_set_input_callback(view, puff_stats_view_input);
    return view;
}

void puff_stats_view_free(View* view) {
    view_free(view);
}
