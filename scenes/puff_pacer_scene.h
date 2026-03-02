#pragma once

#include <gui/scene_manager.h>

typedef enum {
    PuffPacerSceneMainMenu,
    PuffPacerSceneSettings,
    PuffPacerSceneSession,
    PuffPacerSceneDone,
    PuffPacerSceneCount,
} PuffPacerScene;

typedef enum {
    PuffPacerCustomEventMenuSelected,
    PuffPacerCustomEventTimerTick,
    PuffPacerCustomEventSessionToggle,
    PuffPacerCustomEventSessionDone,
} PuffPacerCustomEvent;

extern const SceneManagerHandlers puff_pacer_scene_handlers;
