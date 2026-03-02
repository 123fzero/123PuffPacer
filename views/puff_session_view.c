#include "puff_session_view.h"
#include "../puff_pacer_app.h"
#include <gui/elements.h>
#include <stdio.h>

static void puff_session_view_draw(Canvas* canvas, void* model) {
    PuffSessionModel* m = model;
    canvas_clear(canvas);

    // Header: "PuffPacer" left, elapsed time right
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, "PuffPacer");

    char time_buf[16];
    uint32_t mins = m->total_elapsed / 60;
    uint32_t secs = m->total_elapsed % 60;
    snprintf(time_buf, sizeof(time_buf), "%02lu:%02lu", (unsigned long)mins, (unsigned long)secs);
    canvas_draw_str_aligned(canvas, 126, 10, AlignRight, AlignBottom, time_buf);

    // Puff counter: "Puff 3 / 14"
    canvas_set_font(canvas, FontPrimary);
    char puff_buf[24];
    snprintf(
        puff_buf,
        sizeof(puff_buf),
        "Puff %lu / %lu",
        (unsigned long)m->current_puff,
        (unsigned long)m->total_puffs);
    canvas_draw_str_aligned(canvas, 64, 28, AlignCenter, AlignBottom, puff_buf);

    // Countdown or PAUSED
    canvas_set_font(canvas, FontSecondary);
    if(m->paused) {
        canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignBottom, "PAUSED");
    } else {
        char next_buf[16];
        snprintf(next_buf, sizeof(next_buf), "Next in: %lus", (unsigned long)m->seconds_remaining);
        canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignBottom, next_buf);
    }

    // Progress bar
    uint8_t bar_x = 14;
    uint8_t bar_y = 44;
    uint8_t bar_w = 100;
    uint8_t bar_h = 8;
    elements_frame(canvas, bar_x, bar_y, bar_w, bar_h);
    if(m->total_puffs > 0) {
        uint8_t fill_w = (uint8_t)((m->current_puff - 1) * (bar_w - 2) / m->total_puffs);
        if(fill_w > 0) {
            canvas_draw_box(canvas, bar_x + 1, bar_y + 1, fill_w, bar_h - 2);
        }
    }

    // Hints
    canvas_set_font(canvas, FontSecondary);
    if(m->paused) {
        canvas_draw_str(canvas, 2, 62, "[OK]=Resume");
    } else {
        canvas_draw_str(canvas, 2, 62, "[OK]=Pause");
    }
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, "[<]=Exit");
}

static bool puff_session_view_input(InputEvent* event, void* context) {
    PuffPacerApp* app = context;
    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        // Toggle pause
        bool is_paused = false;
        with_view_model(
            app->session_view,
            PuffSessionModel * m,
            {
                m->paused = !m->paused;
                is_paused = m->paused;
            },
            true);
        if(is_paused) {
            if(app->timer) furi_timer_stop(app->timer);
        } else {
            if(app->timer) furi_timer_start(app->timer, furi_kernel_get_tick_frequency());
        }
        return true;
    }
    return false;
}

View* puff_session_view_alloc(void) {
    View* view = view_alloc();
    view_allocate_model(view, ViewModelTypeLocking, sizeof(PuffSessionModel));
    view_set_draw_callback(view, puff_session_view_draw);
    view_set_input_callback(view, puff_session_view_input);
    return view;
}

void puff_session_view_free(View* view) {
    view_free(view);
}
