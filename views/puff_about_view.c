#include "puff_about_view.h"
#include "../fonts/puff_pacer_font_cyrillic.h"
#include "../puff_pacer_i18n.h"
#include "../puff_pacer_version.h"
#include <gui/canvas.h>
#include <gui/elements.h>
#include <stdio.h>

typedef struct {
    uint8_t scroll;
    uint8_t language;
} PuffAboutModel;

static const uint8_t about_scroll_step = 6;
static const uint8_t about_content_height = 200;

static const uint8_t repo_qr_37x37[] = {
    0xff, 0xb0, 0xd0, 0xf3, 0x1f, 0x81, 0x34, 0x28, 0x12, 0x18, 0xbd, 0x80, 0xce, 0xd1, 0x1b, 0xbd,
    0xbc, 0x3f, 0xd2, 0x1b, 0xbd, 0xbc, 0x3f, 0xd2, 0x1b, 0xbd, 0x08, 0x38, 0xd1, 0x1b, 0x81, 0xcc,
    0xe9, 0x10, 0x18, 0xff, 0xb4, 0xd6, 0xf2, 0x1f, 0x00, 0x78, 0xd6, 0x01, 0x00, 0x00, 0x78, 0xd6,
    0x01, 0x00, 0xbf, 0x4f, 0x2f, 0xae, 0x05, 0x78, 0xb4, 0xd0, 0x71, 0x18, 0x80, 0x33, 0x0e, 0x42,
    0x00, 0x80, 0x33, 0x0e, 0x42, 0x00, 0x5c, 0x80, 0x16, 0x80, 0x05, 0xbd, 0xbb, 0x2f, 0x82, 0x03,
    0x59, 0x0b, 0x16, 0x7d, 0x18, 0xc2, 0xcc, 0xf9, 0xce, 0x03, 0xc2, 0xcc, 0xf9, 0xce, 0x03, 0x67,
    0x47, 0xd8, 0x7f, 0x04, 0xe2, 0x77, 0x38, 0xad, 0x03, 0x79, 0xb7, 0xf1, 0x7d, 0x1a, 0xbd, 0x0f,
    0x2f, 0x72, 0x02, 0xbd, 0x0f, 0x2f, 0x72, 0x02, 0x41, 0xb0, 0x09, 0x1e, 0x04, 0xa5, 0xb3, 0xfe,
    0x7f, 0x1e, 0x00, 0x0c, 0xf8, 0xc2, 0x1f, 0x00, 0x0c, 0xf8, 0xc2, 0x1f, 0xff, 0xcc, 0x38, 0xd3,
    0x03, 0x81, 0x48, 0x06, 0x43, 0x04, 0xbd, 0x7c, 0xf8, 0x7e, 0x1e, 0xbd, 0x8c, 0x07, 0x83, 0x1f,
    0xbd, 0x8c, 0x07, 0x83, 0x1f, 0xbd, 0x0c, 0xcf, 0xff, 0x07, 0x81, 0x8c, 0x01, 0xae, 0x05, 0xff,
    0x84, 0xfe, 0x71, 0x02, 0xff, 0x84, 0xfe, 0x71, 0x02,
};

static void puff_about_set_text_font(Canvas* canvas, PuffPacerLanguage language, Font fallback) {
    if(language == PuffPacerLanguageRu) {
        canvas_set_custom_u8g2_font(canvas, puff_pacer_font_cyrillic);
    } else {
        canvas_set_font(canvas, fallback);
    }
}

static void puff_about_view_draw(Canvas* canvas, void* model) {
    PuffAboutModel* m = model;
    PuffPacerLanguage language = m->language;
    int16_t y = -(int16_t)m->scroll;
    canvas_clear(canvas);

    // Top section: QR first
    puff_about_set_text_font(canvas, language, FontPrimary);
    canvas_draw_str(canvas, 4, y + 18, "123PuffPacer");
    puff_about_set_text_font(canvas, language, FontSecondary);
    canvas_draw_str(canvas, 4, y + 30, PUFF_PACER_VERSION_STR);

    // Keep clear white margin around QR to improve camera recognition.
    canvas_draw_box(canvas, 80, y + 8, 48, 42);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 81, y + 9, 46, 40);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_xbm(canvas, 85, y + 11, 37, 37, repo_qr_37x37);

    // Scrollable details section
    puff_about_set_text_font(canvas, language, FontSecondary);
    // Text next to QR is kept in the left column, then continues full width below QR.
    canvas_draw_str(canvas, 2, y + 42, puff_pacer_i18n(language, PuffPacerTextAboutLine1));
    canvas_draw_str(canvas, 2, y + 52, puff_pacer_i18n(language, PuffPacerTextAboutLine2));

    canvas_draw_str(canvas, 2, y + 66, puff_pacer_i18n(language, PuffPacerTextAboutLine3));
    canvas_draw_str(canvas, 2, y + 80, puff_pacer_i18n(language, PuffPacerTextAboutLine4));
    canvas_draw_str(canvas, 2, y + 96, puff_pacer_i18n(language, PuffPacerTextAboutLine5));
    canvas_draw_str(canvas, 2, y + 110, puff_pacer_i18n(language, PuffPacerTextAboutLine6));
    canvas_draw_str(canvas, 2, y + 120, puff_pacer_i18n(language, PuffPacerTextAboutLine7));
    canvas_draw_str(canvas, 2, y + 134, puff_pacer_i18n(language, PuffPacerTextAboutLine8));
    canvas_draw_str(canvas, 2, y + 148, puff_pacer_i18n(language, PuffPacerTextAboutLine9));
    canvas_draw_str(canvas, 2, y + 158, puff_pacer_i18n(language, PuffPacerTextAboutLine10));
    canvas_draw_str(canvas, 2, y + 168, puff_pacer_i18n(language, PuffPacerTextAboutLine11));

}

static bool puff_about_view_input(InputEvent* event, void* context) {
    UNUSED(context);

    if(event->key != InputKeyUp && event->key != InputKeyDown) {
        return false;
    }

    if(event->type != InputTypeShort && event->type != InputTypeRepeat) {
        return false;
    }

    bool handled = false;
    with_view_model(
        (View*)context,
        PuffAboutModel * m,
        {
            uint8_t max_scroll = about_content_height > 64 ? about_content_height - 64 : 0;
            if(event->key == InputKeyDown && m->scroll < max_scroll) {
                uint16_t next = m->scroll + about_scroll_step;
                m->scroll = next > max_scroll ? max_scroll : next;
                handled = true;
            } else if(event->key == InputKeyUp && m->scroll > 0) {
                m->scroll = m->scroll > about_scroll_step ? m->scroll - about_scroll_step : 0;
                handled = true;
            }
        },
        true);

    return handled;
}

View* puff_about_view_alloc(void) {
    View* view = view_alloc();
    view_allocate_model(view, ViewModelTypeLocking, sizeof(PuffAboutModel));
    with_view_model(
        view,
        PuffAboutModel * m,
        {
            m->scroll = 0;
            m->language = PuffPacerLanguageEn;
        },
        true);
    view_set_draw_callback(view, puff_about_view_draw);
    view_set_input_callback(view, puff_about_view_input);
    view_set_context(view, view);
    return view;
}

void puff_about_view_free(View* view) {
    view_free(view);
}

void puff_about_view_reset(View* view) {
    with_view_model(
        view,
        PuffAboutModel * m,
        {
            m->scroll = 0;
        },
        true);
}

void puff_about_view_set_language(View* view, PuffPacerLanguage language) {
    with_view_model(
        view,
        PuffAboutModel * m,
        {
            m->language = language;
        },
        true);
}
