#pragma once

#include <gui/scene_manager.h>

typedef enum {
    PuffPacerSceneMainMenu,
    PuffPacerSceneSettings,
    PuffPacerSceneStats,
    PuffPacerSceneResetSettingsConfirm,
    PuffPacerSceneResetStatsConfirm,
    PuffPacerSceneSession,
    PuffPacerSceneDone,
    PuffPacerSceneAbout,
    PuffPacerSceneCount,
} PuffPacerScene;

typedef enum {
    PuffPacerCustomEventMenuSelected,
    PuffPacerCustomEventTimerTick,
    PuffPacerCustomEventResetSettings,
    PuffPacerCustomEventConfirmResetSettings,
    PuffPacerCustomEventCancelResetSettings,
    PuffPacerCustomEventResetStats,
    PuffPacerCustomEventConfirmResetStats,
    PuffPacerCustomEventCancelResetStats,
} PuffPacerCustomEvent;

extern const SceneManagerHandlers puff_pacer_scene_handlers;
