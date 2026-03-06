#include "../puff_pacer_app.h"
#include "../puff_pacer_i18n.h"
#include "puff_pacer_scene.h"

static void puff_pacer_scene_reset_stats_confirm_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    PuffPacerApp* app = context;

    if(type != InputTypeShort) {
        return;
    }

    if(result == GuiButtonTypeRight) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, PuffPacerCustomEventConfirmResetStats);
    } else if(result == GuiButtonTypeLeft) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, PuffPacerCustomEventCancelResetStats);
    }
}

void puff_pacer_scene_reset_stats_confirm_on_enter(void* context) {
    PuffPacerApp* app = context;
    PuffPacerLanguage language = app->settings.language;

    widget_reset(app->widget);
    widget_add_string_multiline_element(
        app->widget,
        64,
        18,
        AlignCenter,
        AlignCenter,
        FontPrimary,
        puff_pacer_i18n(language, PuffPacerTextResetStatsTitle));
    widget_add_string_multiline_element(
        app->widget,
        64,
        42,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        puff_pacer_i18n(language, PuffPacerTextResetStatsBody));
    widget_add_button_element(
        app->widget,
        GuiButtonTypeLeft,
        puff_pacer_i18n(language, PuffPacerTextCommonNo),
        puff_pacer_scene_reset_stats_confirm_callback,
        app);
    widget_add_button_element(
        app->widget,
        GuiButtonTypeRight,
        puff_pacer_i18n(language, PuffPacerTextCommonYes),
        puff_pacer_scene_reset_stats_confirm_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewWidget);
}

bool puff_pacer_scene_reset_stats_confirm_on_event(void* context, SceneManagerEvent event) {
    PuffPacerApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PuffPacerCustomEventConfirmResetStats) {
            puff_pacer_stats_reset(&app->stats);
            puff_pacer_stats_save(&app->stats);
            notification_message(app->notifications, &sequence_success);
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, PuffPacerSceneSettings);
            return true;
        } else if(event.event == PuffPacerCustomEventCancelResetStats) {
            scene_manager_previous_scene(app->scene_manager);
            return true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        return true;
    }

    return false;
}

void puff_pacer_scene_reset_stats_confirm_on_exit(void* context) {
    PuffPacerApp* app = context;
    widget_reset(app->widget);
}
