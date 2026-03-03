#pragma once

#include <gui/scene_manager.h>

typedef enum {
    PuffPacerSceneMainMenu,
    PuffPacerSceneSettings,
    PuffPacerSceneSession,
    PuffPacerSceneDone,
    PuffPacerSceneAbout,
    PuffPacerSceneCount,
} PuffPacerScene;

typedef enum {
    PuffPacerCustomEventMenuSelected,
    PuffPacerCustomEventTimerTick,
    PuffPacerCustomEventResetSettings,
} PuffPacerCustomEvent;

extern const SceneManagerHandlers puff_pacer_scene_handlers;
