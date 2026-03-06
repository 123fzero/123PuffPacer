#include "../puff_pacer_app.h"
#include "../views/puff_about_view.h"
#include "puff_pacer_scene.h"

void puff_pacer_scene_about_on_enter(void* context) {
    PuffPacerApp* app = context;
    puff_about_view_set_language(app->about_view, app->settings.language);
    puff_about_view_reset(app->about_view);
    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewAbout);
}

bool puff_pacer_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void puff_pacer_scene_about_on_exit(void* context) {
    UNUSED(context);
}
