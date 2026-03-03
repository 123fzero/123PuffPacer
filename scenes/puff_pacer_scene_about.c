#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

void puff_pacer_scene_about_on_enter(void* context) {
    PuffPacerApp* app = context;

    widget_reset(app->widget);
    widget_add_text_scroll_element(
        app->widget,
        0,
        0,
        128,
        64,
        "123PuffPacer v1.1.1\n"
        "Flipper Zero puff timer\n"
        "for heated tobacco\n\n"
        "IQOS, HEETS, TEREA,\n"
        "Lil Solid, glo, Ploom\n\n"
        "Author: 123fzero\n"
        "Year: 2026\n\n"
        "github.com/123fzero\n"
        "/123PuffPacer");

    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewWidget);
}

bool puff_pacer_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void puff_pacer_scene_about_on_exit(void* context) {
    PuffPacerApp* app = context;
    widget_reset(app->widget);
}
