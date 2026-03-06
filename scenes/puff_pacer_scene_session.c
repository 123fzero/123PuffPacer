#include "../puff_pacer_app.h"
#include "../views/puff_session_view.h"
#include "puff_pacer_scene.h"
#include <datetime/datetime.h>
#include <furi_hal_rtc.h>

// Vibration sequences
static const NotificationSequence sequence_vibro_short = {
    &message_vibro_on,
    &message_delay_100,
    &message_vibro_off,
    NULL,
};

static const NotificationSequence sequence_vibro_long = {
    &message_vibro_on,
    &message_delay_500,
    &message_vibro_off,
    NULL,
};

// Sound sequence
static const NotificationSequence sequence_beep = {
    &message_note_c7,
    &message_delay_100,
    &message_sound_off,
    NULL,
};

static void puff_pacer_send_alert(PuffPacerApp* app) {
    // Vibration
    if(app->settings.vibro_mode == PuffPacerVibroShort) {
        notification_message(app->notifications, &sequence_vibro_short);
    } else if(app->settings.vibro_mode == PuffPacerVibroLong) {
        notification_message(app->notifications, &sequence_vibro_long);
    }

    // Sound
    if(app->settings.sound_mode == PuffPacerSoundOn) {
        notification_message(app->notifications, &sequence_beep);
    }
}

static void puff_pacer_session_timer_callback(void* context) {
    PuffPacerApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, PuffPacerCustomEventTimerTick);
}

static void puff_pacer_session_get_datetime_keys(uint32_t* date_key, uint32_t* time_key) {
    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    *date_key = ((uint32_t)dt.year * 10000U) + ((uint32_t)dt.month * 100U) + dt.day;
    *time_key = ((uint32_t)dt.hour * 100U) + dt.minute;
}

void puff_pacer_scene_session_on_enter(void* context) {
    PuffPacerApp* app = context;

    puff_pacer_stats_log_started(&app->stats);
    puff_pacer_stats_save(&app->stats);

    // Initialize session model
    with_view_model(
        app->session_view,
        PuffSessionModel * m,
        {
            m->current_puff = 1;
            m->total_puffs = app->settings.puff_count;
            m->seconds_remaining = app->settings.interval_sec;
            m->total_elapsed = 0;
            m->paused = false;
            m->smoke_timer = 5;
            m->smoke_frame = 0;
        },
        true);

    // First puff notification
    puff_pacer_send_alert(app);

    // Start timer
    app->timer = furi_timer_alloc(puff_pacer_session_timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(app->timer, furi_kernel_get_tick_frequency());

    // Keep backlight on
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);

    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewSession);
}

bool puff_pacer_scene_session_on_event(void* context, SceneManagerEvent event) {
    PuffPacerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PuffPacerCustomEventTimerTick) {
            bool session_done = false;
            bool new_puff = false;
            with_view_model(
                app->session_view,
                PuffSessionModel * m,
                {
                    m->total_elapsed++;
                    if(m->smoke_timer > 0) {
                        m->smoke_timer--;
                        m->smoke_frame++;
                    }
                    if(m->seconds_remaining > 0) {
                        m->seconds_remaining--;
                    }
                    if(m->seconds_remaining == 0) {
                        if(m->current_puff >= m->total_puffs) {
                            session_done = true;
                        } else {
                            m->current_puff++;
                            m->seconds_remaining = app->settings.interval_sec;
                            m->smoke_timer = 5;
                            m->smoke_frame = 0;
                            new_puff = true;
                        }
                    }
                },
                true);

            if(session_done) {
                furi_timer_stop(app->timer);
                uint32_t date_key;
                uint32_t time_key;
                puff_pacer_session_get_datetime_keys(&date_key, &time_key);
                puff_pacer_stats_log_completed(
                    &app->stats,
                    date_key,
                    time_key,
                    app->settings.puff_count,
                    app->settings.interval_sec);
                puff_pacer_stats_save(&app->stats);
                notification_message(app->notifications, &sequence_success);
                scene_manager_next_scene(app->scene_manager, PuffPacerSceneDone);
            } else if(new_puff) {
                puff_pacer_send_alert(app);
            }
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(app->timer) {
            furi_timer_stop(app->timer);
        }
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, PuffPacerSceneMainMenu);
        consumed = true;
    }

    return consumed;
}

void puff_pacer_scene_session_on_exit(void* context) {
    PuffPacerApp* app = context;

    // Restore backlight
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);

    // Stop and free timer
    if(app->timer) {
        furi_timer_stop(app->timer);
        furi_timer_free(app->timer);
        app->timer = NULL;
    }
}
