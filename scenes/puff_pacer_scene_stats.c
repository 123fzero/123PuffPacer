#include "../puff_pacer_app.h"
#include "../views/puff_stats_view.h"
#include "puff_pacer_scene.h"

void puff_pacer_scene_stats_on_enter(void* context) {
    PuffPacerApp* app = context;

    with_view_model(
        app->stats_view,
        PuffStatsModel * m,
        {
            m->stats = app->stats;
            m->screen = 0;
            m->log_offset = 0;
            m->day_index = 6;
            m->language = app->settings.language;
            m->hourly_date_key = 0;
        },
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewStats);
}

bool puff_pacer_scene_stats_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void puff_pacer_scene_stats_on_exit(void* context) {
    UNUSED(context);
}
