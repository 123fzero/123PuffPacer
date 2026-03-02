#include "puff_pacer_app.h"
#include "scenes/puff_pacer_scene.h"
#include "views/puff_session_view.h"

static bool puff_pacer_custom_event_callback(void* context, uint32_t event) {
    PuffPacerApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool puff_pacer_back_event_callback(void* context) {
    PuffPacerApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static PuffPacerApp* puff_pacer_alloc(void) {
    PuffPacerApp* app = malloc(sizeof(PuffPacerApp));

    // Load settings
    puff_pacer_settings_load(&app->settings);

    // Scene manager
    app->scene_manager = scene_manager_alloc(&puff_pacer_scene_handlers, app);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, puff_pacer_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, puff_pacer_back_event_callback);

    // Submenu
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, PuffPacerViewSubmenu, submenu_get_view(app->submenu));

    // Variable item list
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        PuffPacerViewVariableItemList,
        variable_item_list_get_view(app->variable_item_list));

    // Widget (for Done screen)
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, PuffPacerViewWidget, widget_get_view(app->widget));

    // Notifications
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Timer (will be started/stopped per session)
    app->timer = NULL;

    // Session view
    app->session_view = puff_session_view_alloc();
    view_set_context(app->session_view, app);
    view_dispatcher_add_view(app->view_dispatcher, PuffPacerViewSession, app->session_view);

    return app;
}

static void puff_pacer_free(PuffPacerApp* app) {
    // Timer
    if(app->timer) {
        furi_timer_stop(app->timer);
        furi_timer_free(app->timer);
    }

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, PuffPacerViewSession);
    puff_session_view_free(app->session_view);

    view_dispatcher_remove_view(app->view_dispatcher, PuffPacerViewSubmenu);
    submenu_free(app->submenu);

    view_dispatcher_remove_view(app->view_dispatcher, PuffPacerViewVariableItemList);
    variable_item_list_free(app->variable_item_list);

    view_dispatcher_remove_view(app->view_dispatcher, PuffPacerViewWidget);
    widget_free(app->widget);

    // Scene manager and dispatcher
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    // Records
    furi_record_close(RECORD_NOTIFICATION);

    free(app);
}

int32_t puff_pacer_app(void* p) {
    UNUSED(p);
    PuffPacerApp* app = puff_pacer_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, PuffPacerSceneMainMenu);
    view_dispatcher_run(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    puff_pacer_free(app);
    return 0;
}
