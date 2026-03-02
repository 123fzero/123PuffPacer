#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

enum {
    PuffPacerMainMenuIndexStart,
    PuffPacerMainMenuIndexSettings,
};

static void puff_pacer_scene_main_menu_callback(void* context, uint32_t index) {
    PuffPacerApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void puff_pacer_scene_main_menu_on_enter(void* context) {
    PuffPacerApp* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "PuffPacer");
    submenu_add_item(
        app->submenu,
        "Start Session",
        PuffPacerMainMenuIndexStart,
        puff_pacer_scene_main_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Settings",
        PuffPacerMainMenuIndexSettings,
        puff_pacer_scene_main_menu_callback,
        app);
    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, PuffPacerSceneMainMenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewSubmenu);
}

bool puff_pacer_scene_main_menu_on_event(void* context, SceneManagerEvent event) {
    PuffPacerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, PuffPacerSceneMainMenu, event.event);
        switch(event.event) {
        case PuffPacerMainMenuIndexStart:
            scene_manager_next_scene(app->scene_manager, PuffPacerSceneSession);
            consumed = true;
            break;
        case PuffPacerMainMenuIndexSettings:
            scene_manager_next_scene(app->scene_manager, PuffPacerSceneSettings);
            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        consumed = true;
    }

    return consumed;
}

void puff_pacer_scene_main_menu_on_exit(void* context) {
    PuffPacerApp* app = context;
    submenu_reset(app->submenu);
}
