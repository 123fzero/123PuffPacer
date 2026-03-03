#include "puff_session_view.h"
#include "../puff_pacer_app.h"
#include <gui/elements.h>
#include <stdio.h>

// S-shaped smoke wisp: each wisp is an array of (dx, dy) offsets from base position
// Drawn relative to a base (x, y) point
typedef struct {
    int8_t dx;
    int8_t dy;
} SmokePixel;

// Wisp shape 0: S-curve going right-left-right
static const SmokePixel wisp_shape_0[] = {
    {0, 0},
    {1, -1},
    {2, -2},
    {1, -3},
    {0, -4},
    {1, -5},
};
#define WISP_0_LEN 6

// Wisp shape 1: S-curve going left-right-left
static const SmokePixel wisp_shape_1[] = {
    {0, 0},
    {-1, -1},
    {-1, -2},
    {0, -3},
    {1, -4},
    {0, -5},
};
#define WISP_1_LEN 6

// Wisp shape 2: tighter curve
static const SmokePixel wisp_shape_2[] = {
    {0, 0},
    {1, -1},
    {0, -2},
    {-1, -3},
    {0, -4},
};
#define WISP_2_LEN 5

// Smoke wisp definition: start position near center + shape
typedef struct {
    int8_t start_x; // offset from center (64) — negative=left, positive=right
    int8_t start_y; // start Y position near middle
    uint8_t shape;  // 0, 1, or 2
} SmokeWisp;

// Wisps start in side margins (outside text area) and spread outward+upward
// Text occupies roughly x=14..114, so wisps start at x ~12-20 (left) and ~108-116 (right)
// Left group: start_x offsets from center (64)
static const SmokeWisp left_wisps[] = {
    {-46, 42, 0},
    {-50, 36, 1},
    {-44, 30, 2},
    {-48, 24, 0},
    {-52, 18, 1},
    {-45, 46, 2},
    {-49, 40, 0},
    {-47, 34, 1},
};
#define LEFT_WISP_COUNT 8

// Right group: mirrored
static const SmokeWisp right_wisps[] = {
    {46, 42, 1},
    {50, 36, 0},
    {44, 30, 2},
    {48, 24, 1},
    {52, 18, 0},
    {45, 46, 2},
    {49, 40, 1},
    {47, 34, 0},
};
#define RIGHT_WISP_COUNT 8

static void draw_wisp(Canvas* canvas, int16_t x, int16_t y, uint8_t shape) {
    const SmokePixel* pixels;
    uint8_t len;
    switch(shape) {
    case 0:
        pixels = wisp_shape_0;
        len = WISP_0_LEN;
        break;
    case 1:
        pixels = wisp_shape_1;
        len = WISP_1_LEN;
        break;
    default:
        pixels = wisp_shape_2;
        len = WISP_2_LEN;
        break;
    }
    for(uint8_t i = 0; i < len; i++) {
        int16_t px = x + pixels[i].dx;
        int16_t py = y + pixels[i].dy;
        if(px >= 0 && px < 128 && py >= 0 && py < 64) {
            canvas_draw_dot(canvas, px, py);
        }
    }
}

static void draw_smoke(Canvas* canvas, uint8_t frame) {
    // Each frame wisps spread diagonally: outward (X) and upward (Y)
    // Left wisps move left and up, right wisps move right and up
    int16_t spread = (int16_t)frame * 3;  // horizontal spread per frame
    int16_t rise = (int16_t)frame * 2;    // vertical rise per frame

    for(uint8_t i = 0; i < LEFT_WISP_COUNT; i++) {
        int16_t x = 64 + (int16_t)left_wisps[i].start_x - spread;
        int16_t y = (int16_t)left_wisps[i].start_y - rise;
        if(x > -6 && x < 128 && y > -6 && y < 64) {
            draw_wisp(canvas, x, y, left_wisps[i].shape);
        }
    }
    for(uint8_t i = 0; i < RIGHT_WISP_COUNT; i++) {
        int16_t x = 64 + (int16_t)right_wisps[i].start_x + spread;
        int16_t y = (int16_t)right_wisps[i].start_y - rise;
        if(x > -6 && x < 128 && y > -6 && y < 64) {
            draw_wisp(canvas, x, y, right_wisps[i].shape);
        }
    }
}

static void puff_session_view_draw(Canvas* canvas, void* model) {
    PuffSessionModel* m = model;
    canvas_clear(canvas);

    // Header: "PuffPacer" left, elapsed time right
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, "123PuffPacer");

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

    // Smoke animation
    if(m->smoke_timer > 0) {
        draw_smoke(canvas, m->smoke_frame);
    }
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
