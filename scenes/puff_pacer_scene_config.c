#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

// Forward declarations
void puff_pacer_scene_main_menu_on_enter(void*);
bool puff_pacer_scene_main_menu_on_event(void*, SceneManagerEvent);
void puff_pacer_scene_main_menu_on_exit(void*);

void puff_pacer_scene_settings_on_enter(void*);
bool puff_pacer_scene_settings_on_event(void*, SceneManagerEvent);
void puff_pacer_scene_settings_on_exit(void*);

void puff_pacer_scene_session_on_enter(void*);
bool puff_pacer_scene_session_on_event(void*, SceneManagerEvent);
void puff_pacer_scene_session_on_exit(void*);

void puff_pacer_scene_done_on_enter(void*);
bool puff_pacer_scene_done_on_event(void*, SceneManagerEvent);
void puff_pacer_scene_done_on_exit(void*);

void (*const puff_pacer_scene_on_enter_handlers[])(void*) = {
    puff_pacer_scene_main_menu_on_enter,
    puff_pacer_scene_settings_on_enter,
    puff_pacer_scene_session_on_enter,
    puff_pacer_scene_done_on_enter,
};

bool (*const puff_pacer_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    puff_pacer_scene_main_menu_on_event,
    puff_pacer_scene_settings_on_event,
    puff_pacer_scene_session_on_event,
    puff_pacer_scene_done_on_event,
};

void (*const puff_pacer_scene_on_exit_handlers[])(void*) = {
    puff_pacer_scene_main_menu_on_exit,
    puff_pacer_scene_settings_on_exit,
    puff_pacer_scene_session_on_exit,
    puff_pacer_scene_done_on_exit,
};

const SceneManagerHandlers puff_pacer_scene_handlers = {
    .on_enter_handlers = puff_pacer_scene_on_enter_handlers,
    .on_event_handlers = puff_pacer_scene_on_event_handlers,
    .on_exit_handlers = puff_pacer_scene_on_exit_handlers,
    .scene_num = PuffPacerSceneCount,
};
