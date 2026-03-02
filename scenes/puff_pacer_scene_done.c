#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

static void
    puff_pacer_scene_done_button_callback(GuiButtonType result, InputType type, void* context) {
    PuffPacerApp* app = context;
    if(type == InputTypeShort && result == GuiButtonTypeCenter) {
        view_dispatcher_send_custom_event(app->view_dispatcher, PuffPacerCustomEventMenuSelected);
    }
}

void puff_pacer_scene_done_on_enter(void* context) {
    PuffPacerApp* app = context;

    widget_reset(app->widget);
    widget_add_string_multiline_element(
        app->widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, "Session\nComplete!");
    widget_add_button_element(
        app->widget, GuiButtonTypeCenter, "OK", puff_pacer_scene_done_button_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewWidget);
}

bool puff_pacer_scene_done_on_event(void* context, SceneManagerEvent event) {
    PuffPacerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, PuffPacerSceneMainMenu);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, PuffPacerSceneMainMenu);
        consumed = true;
    }

    return consumed;
}

void puff_pacer_scene_done_on_exit(void* context) {
    PuffPacerApp* app = context;
    widget_reset(app->widget);
}
