#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

static void puff_pacer_scene_reset_settings_confirm_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    PuffPacerApp* app = context;

    if(type != InputTypeShort) {
        return;
    }

    if(result == GuiButtonTypeRight) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, PuffPacerCustomEventConfirmResetSettings);
    } else if(result == GuiButtonTypeLeft) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, PuffPacerCustomEventCancelResetSettings);
    }
}

void puff_pacer_scene_reset_settings_confirm_on_enter(void* context) {
    PuffPacerApp* app = context;

    widget_reset(app->widget);
    widget_add_string_multiline_element(
        app->widget, 64, 18, AlignCenter, AlignCenter, FontPrimary, "Reset\nsettings?");
    widget_add_string_multiline_element(
        app->widget, 64, 42, AlignCenter, AlignCenter, FontSecondary, "Puffs, interval,\nalerts to default");
    widget_add_button_element(
        app->widget,
        GuiButtonTypeLeft,
        "No",
        puff_pacer_scene_reset_settings_confirm_callback,
        app);
    widget_add_button_element(
        app->widget,
        GuiButtonTypeRight,
        "Yes",
        puff_pacer_scene_reset_settings_confirm_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewWidget);
}

bool puff_pacer_scene_reset_settings_confirm_on_event(void* context, SceneManagerEvent event) {
    PuffPacerApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PuffPacerCustomEventConfirmResetSettings) {
            app->settings.puff_count = PUFF_COUNT_DEFAULT;
            app->settings.interval_sec = INTERVAL_SEC_DEFAULT;
            app->settings.vibro_mode = PuffPacerVibroShort;
            app->settings.sound_mode = PuffPacerSoundOn;
            puff_pacer_settings_save(&app->settings);
            notification_message(app->notifications, &sequence_success);
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, PuffPacerSceneSettings);
            return true;
        } else if(event.event == PuffPacerCustomEventCancelResetSettings) {
            scene_manager_previous_scene(app->scene_manager);
            return true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        return true;
    }

    return false;
}

void puff_pacer_scene_reset_settings_confirm_on_exit(void* context) {
    PuffPacerApp* app = context;
    widget_reset(app->widget);
}
