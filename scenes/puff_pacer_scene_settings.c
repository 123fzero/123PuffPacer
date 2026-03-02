#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"
#include <stdio.h>

static void puff_pacer_scene_settings_puff_count_changed(VariableItem* item) {
    PuffPacerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint32_t value = PUFF_COUNT_MIN + index;
    app->settings.puff_count = value;

    char buf[8];
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)value);
    variable_item_set_current_value_text(item, buf);

    puff_pacer_settings_save(&app->settings);
}

static void puff_pacer_scene_settings_interval_changed(VariableItem* item) {
    PuffPacerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint32_t value = INTERVAL_SEC_MIN + index;
    app->settings.interval_sec = value;

    char buf[8];
    snprintf(buf, sizeof(buf), "%lus", (unsigned long)value);
    variable_item_set_current_value_text(item, buf);

    puff_pacer_settings_save(&app->settings);
}

void puff_pacer_scene_settings_on_enter(void* context) {
    PuffPacerApp* app = context;
    VariableItem* item;
    char buf[8];

    variable_item_list_reset(app->variable_item_list);

    // Puff Count: 8..20
    uint8_t puff_count_values = PUFF_COUNT_MAX - PUFF_COUNT_MIN + 1;
    item = variable_item_list_add(
        app->variable_item_list,
        "Puff Count",
        puff_count_values,
        puff_pacer_scene_settings_puff_count_changed,
        app);
    uint8_t puff_index = app->settings.puff_count - PUFF_COUNT_MIN;
    variable_item_set_current_value_index(item, puff_index);
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)app->settings.puff_count);
    variable_item_set_current_value_text(item, buf);

    // Interval: 15..40 sec
    uint8_t interval_values = INTERVAL_SEC_MAX - INTERVAL_SEC_MIN + 1;
    item = variable_item_list_add(
        app->variable_item_list,
        "Interval",
        interval_values,
        puff_pacer_scene_settings_interval_changed,
        app);
    uint8_t interval_index = app->settings.interval_sec - INTERVAL_SEC_MIN;
    variable_item_set_current_value_index(item, interval_index);
    snprintf(buf, sizeof(buf), "%lus", (unsigned long)app->settings.interval_sec);
    variable_item_set_current_value_text(item, buf);

    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewVariableItemList);
}

bool puff_pacer_scene_settings_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void puff_pacer_scene_settings_on_exit(void* context) {
    PuffPacerApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
