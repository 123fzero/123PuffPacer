#include "../puff_pacer_app.h"
#include "../puff_pacer_i18n.h"
#include "puff_pacer_scene.h"
#include <stdio.h>

#define SETTINGS_ITEM_RESET       5
#define SETTINGS_ITEM_RESET_STATS 6

static const PuffPacerTextKey language_labels[] = {PuffPacerTextLangEn, PuffPacerTextLangRu};

static const PuffPacerTextKey vibro_labels[][PuffPacerVibroCount] = {
    {PuffPacerTextCommonOff, PuffPacerTextVibroShort, PuffPacerTextVibroLong},
    {PuffPacerTextCommonOff, PuffPacerTextVibroShort, PuffPacerTextVibroLong},
};

static const PuffPacerTextKey sound_labels[][PuffPacerSoundCount] = {
    {PuffPacerTextCommonOff, PuffPacerTextCommonOn},
    {PuffPacerTextCommonOff, PuffPacerTextCommonOn},
};

static void puff_pacer_scene_settings_enter_callback(void* context, uint32_t index);
void puff_pacer_scene_settings_puff_count_changed(VariableItem* item);
void puff_pacer_scene_settings_interval_changed(VariableItem* item);
void puff_pacer_scene_settings_vibro_changed(VariableItem* item);
void puff_pacer_scene_settings_sound_changed(VariableItem* item);
void puff_pacer_scene_settings_language_changed(VariableItem* item);

static void puff_pacer_scene_settings_build(PuffPacerApp* app) {
    VariableItem* item;
    char buf[24];
    PuffPacerLanguage language = app->settings.language;

    variable_item_list_reset(app->variable_item_list);

    // Puff Count: 8..20
    uint8_t puff_count_values = PUFF_COUNT_MAX - PUFF_COUNT_MIN + 1;
    item = variable_item_list_add(
        app->variable_item_list,
        puff_pacer_i18n(language, PuffPacerTextSettingsPuffCount),
        puff_count_values,
        puff_pacer_scene_settings_puff_count_changed,
        app);
    uint8_t puff_index = app->settings.puff_count - PUFF_COUNT_MIN;
    variable_item_set_current_value_index(item, puff_index);
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)app->settings.puff_count);
    variable_item_set_current_value_text(item, buf);

    // Interval: 10..40 sec
    uint8_t interval_values = INTERVAL_SEC_MAX - INTERVAL_SEC_MIN + 1;
    item = variable_item_list_add(
        app->variable_item_list,
        puff_pacer_i18n(language, PuffPacerTextSettingsInterval),
        interval_values,
        puff_pacer_scene_settings_interval_changed,
        app);
    uint8_t interval_index = app->settings.interval_sec - INTERVAL_SEC_MIN;
    variable_item_set_current_value_index(item, interval_index);
    snprintf(buf, sizeof(buf), "%lus", (unsigned long)app->settings.interval_sec);
    variable_item_set_current_value_text(item, buf);

    // Vibration: Off / Short / Long
    item = variable_item_list_add(
        app->variable_item_list,
        puff_pacer_i18n(language, PuffPacerTextSettingsVibration),
        PuffPacerVibroCount,
        puff_pacer_scene_settings_vibro_changed,
        app);
    variable_item_set_current_value_index(item, app->settings.vibro_mode);
    variable_item_set_current_value_text(
        item,
        puff_pacer_i18n(language, vibro_labels[language][app->settings.vibro_mode]));

    // Sound: Off / On
    item = variable_item_list_add(
        app->variable_item_list,
        puff_pacer_i18n(language, PuffPacerTextSettingsSound),
        PuffPacerSoundCount,
        puff_pacer_scene_settings_sound_changed,
        app);
    variable_item_set_current_value_index(item, app->settings.sound_mode);
    variable_item_set_current_value_text(
        item,
        puff_pacer_i18n(language, sound_labels[language][app->settings.sound_mode]));

    // Language: EN / RU
    item = variable_item_list_add(
        app->variable_item_list,
        puff_pacer_i18n(language, PuffPacerTextSettingsLanguage),
        PuffPacerLanguageCount,
        puff_pacer_scene_settings_language_changed,
        app);
    variable_item_set_current_value_index(item, app->settings.language);
    variable_item_set_current_value_text(
        item,
        puff_pacer_i18n(language, language_labels[app->settings.language]));

    // Reset items
    variable_item_list_add(
        app->variable_item_list,
        puff_pacer_i18n(language, PuffPacerTextSettingsReset),
        0,
        NULL,
        app);
    variable_item_list_add(
        app->variable_item_list,
        puff_pacer_i18n(language, PuffPacerTextSettingsResetStats),
        0,
        NULL,
        app);

    variable_item_list_set_enter_callback(
        app->variable_item_list, puff_pacer_scene_settings_enter_callback, app);
}

static void puff_pacer_scene_settings_enter_callback(void* context, uint32_t index) {
    PuffPacerApp* app = context;
    if(index == SETTINGS_ITEM_RESET) {
        view_dispatcher_send_custom_event(app->view_dispatcher, PuffPacerCustomEventResetSettings);
    } else if(index == SETTINGS_ITEM_RESET_STATS) {
        view_dispatcher_send_custom_event(app->view_dispatcher, PuffPacerCustomEventResetStats);
    }
}

void puff_pacer_scene_settings_puff_count_changed(VariableItem* item) {
    PuffPacerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint32_t value = PUFF_COUNT_MIN + index;
    app->settings.puff_count = value;

    char buf[8];
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)value);
    variable_item_set_current_value_text(item, buf);

    puff_pacer_settings_save(&app->settings);
}

void puff_pacer_scene_settings_interval_changed(VariableItem* item) {
    PuffPacerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint32_t value = INTERVAL_SEC_MIN + index;
    app->settings.interval_sec = value;

    char buf[8];
    snprintf(buf, sizeof(buf), "%lus", (unsigned long)value);
    variable_item_set_current_value_text(item, buf);

    puff_pacer_settings_save(&app->settings);
}

void puff_pacer_scene_settings_vibro_changed(VariableItem* item) {
    PuffPacerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.vibro_mode = index;

    variable_item_set_current_value_text(
        item,
        puff_pacer_i18n(app->settings.language, vibro_labels[app->settings.language][index]));

    puff_pacer_settings_save(&app->settings);
}

void puff_pacer_scene_settings_sound_changed(VariableItem* item) {
    PuffPacerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.sound_mode = index;

    variable_item_set_current_value_text(
        item,
        puff_pacer_i18n(app->settings.language, sound_labels[app->settings.language][index]));

    puff_pacer_settings_save(&app->settings);
}

void puff_pacer_scene_settings_language_changed(VariableItem* item) {
    PuffPacerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.language = index;

    variable_item_set_current_value_text(
        item,
        puff_pacer_i18n(app->settings.language, language_labels[app->settings.language]));

    puff_pacer_settings_save(&app->settings);
    view_dispatcher_send_custom_event(app->view_dispatcher, PuffPacerCustomEventLanguageChanged);
}

void puff_pacer_scene_settings_on_enter(void* context) {
    PuffPacerApp* app = context;
    puff_pacer_scene_settings_build(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewVariableItemList);
}

bool puff_pacer_scene_settings_on_event(void* context, SceneManagerEvent event) {
    PuffPacerApp* app = context;
    if(event.type == SceneManagerEventTypeCustom &&
       event.event == PuffPacerCustomEventResetSettings) {
        scene_manager_next_scene(app->scene_manager, PuffPacerSceneResetSettingsConfirm);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom &&
              event.event == PuffPacerCustomEventResetStats) {
        scene_manager_next_scene(app->scene_manager, PuffPacerSceneResetStatsConfirm);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom &&
              event.event == PuffPacerCustomEventLanguageChanged) {
        puff_pacer_scene_settings_build(app);
        return true;
    }
    return false;
}

void puff_pacer_scene_settings_on_exit(void* context) {
    PuffPacerApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
