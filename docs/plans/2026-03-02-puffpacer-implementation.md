# PuffPacer Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build an interval puff timer app for Flipper Zero with configurable settings, visual countdown, and vibration+beep notifications.

**Architecture:** ViewDispatcher + SceneManager pattern. Native Submenu for main menu, VariableItemList for settings, custom View for session timer screen. FuriTimer for 1-second periodic ticks. FlipperFormat for persistent settings on SD card.

**Tech Stack:** C, Flipper Zero SDK (ufbt), furi_timer, notification_message, FlipperFormat

---

### Task 1: Project scaffold and manifest

**Files:**
- Create: `application.fam`
- Create: `puff_pacer.c` (minimal entry point)
- Create: `icon.png` (10x10 placeholder)

**Step 1: Create application.fam**

```python
App(
    appid="puff_pacer",
    name="PuffPacer",
    apptype=FlipperAppType.EXTERNAL,
    entry_point="puff_pacer_app",
    stack_size=2 * 1024,
    fap_version=(1, 0),
    fap_icon="icon.png",
    fap_category="Tools",
    fap_description="Interval puff timer with vibration alerts",
    fap_author="@colinfrl",
    requires=[
        "gui",
        "notification",
        "storage",
    ],
)
```

**Step 2: Create minimal puff_pacer.c**

```c
#include <furi.h>
#include <gui/gui.h>

int32_t puff_pacer_app(void* p) {
    UNUSED(p);
    FURI_LOG_I("PuffPacer", "Hello from PuffPacer!");
    furi_delay_ms(2000);
    return 0;
}
```

**Step 3: Create placeholder icon.png**

Create a 10x10 pixel 1-bit PNG. Can use ImageMagick:
```bash
convert -size 10x10 xc:black -fill white -draw "circle 5,5 5,1" icon.png
```
Or just create any valid 10x10 1-bit PNG.

**Step 4: Verify build**

```bash
cd /Users/colinfrl/work/123fzero/123PuffPacer && ufbt
```
Expected: Build succeeds, `.fap` file created in `dist/`.

**Step 5: Commit**

```bash
git add application.fam puff_pacer.c icon.png
git commit -m "feat: initial project scaffold with manifest and entry point"
```

---

### Task 2: App struct, settings types, and ViewDispatcher skeleton

**Files:**
- Create: `puff_pacer_app.h`
- Create: `puff_pacer_settings.h`
- Create: `puff_pacer_settings.c`
- Modify: `puff_pacer.c`

**Step 1: Create puff_pacer_settings.h**

```c
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PUFF_COUNT_MIN 8
#define PUFF_COUNT_MAX 20
#define PUFF_COUNT_DEFAULT 10

#define INTERVAL_SEC_MIN 10
#define INTERVAL_SEC_MAX 40
#define INTERVAL_SEC_DEFAULT 30

#define SETTINGS_FILE_PATH APP_DATA_PATH("settings.conf")
#define SETTINGS_FILE_TYPE "PuffPacer Settings"
#define SETTINGS_FILE_VERSION 2

typedef struct {
    uint32_t puff_count;
    uint32_t interval_sec;
} PuffPacerSettings;

bool puff_pacer_settings_load(PuffPacerSettings* settings);
bool puff_pacer_settings_save(const PuffPacerSettings* settings);
```

**Step 2: Create puff_pacer_settings.c**

```c
#include "puff_pacer_settings.h"
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

bool puff_pacer_settings_save(const PuffPacerSettings* settings) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = false;

    do {
        if(!flipper_format_file_open_always(ff, SETTINGS_FILE_PATH)) break;
        if(!flipper_format_write_header_cstr(ff, SETTINGS_FILE_TYPE, SETTINGS_FILE_VERSION))
            break;
        if(!flipper_format_write_uint32(ff, "PuffCount", &settings->puff_count, 1)) break;
        if(!flipper_format_write_uint32(ff, "IntervalSec", &settings->interval_sec, 1)) break;
        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool puff_pacer_settings_load(PuffPacerSettings* settings) {
    settings->puff_count = PUFF_COUNT_DEFAULT;
    settings->interval_sec = INTERVAL_SEC_DEFAULT;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = false;

    do {
        if(!flipper_format_file_open_existing(ff, SETTINGS_FILE_PATH)) break;

        FuriString* filetype = furi_string_alloc();
        uint32_t version = 0;
        if(!flipper_format_read_header(ff, filetype, &version)) {
            furi_string_free(filetype);
            break;
        }
        if(furi_string_cmp_str(filetype, SETTINGS_FILE_TYPE) != 0 ||
           version != SETTINGS_FILE_VERSION) {
            furi_string_free(filetype);
            break;
        }
        furi_string_free(filetype);

        flipper_format_read_uint32(ff, "PuffCount", &settings->puff_count, 1);
        flipper_format_read_uint32(ff, "IntervalSec", &settings->interval_sec, 1);
        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}
```

**Step 3: Create puff_pacer_app.h**

```c
#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>

#include "puff_pacer_settings.h"

typedef enum {
    PuffPacerViewSubmenu,
    PuffPacerViewVariableItemList,
    PuffPacerViewSession,
    PuffPacerViewWidget,
} PuffPacerView;

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    VariableItemList* variable_item_list;
    Widget* widget;
    View* session_view;
    NotificationApp* notifications;
    FuriTimer* timer;
    PuffPacerSettings settings;
} PuffPacerApp;
```

**Step 4: Update puff_pacer.c with alloc/free/run skeleton**

```c
#include "puff_pacer_app.h"
#include "scenes/puff_pacer_scene.h"

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

    return app;
}

static void puff_pacer_free(PuffPacerApp* app) {
    // Timer
    if(app->timer) {
        furi_timer_stop(app->timer);
        furi_timer_free(app->timer);
    }

    // Views
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
```

**Step 5: Create scenes/puff_pacer_scene.h with stub handlers**

```c
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
```

**Step 6: Create scene stubs**

Create `scenes/puff_pacer_scene_main_menu.c`:
```c
#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

void puff_pacer_scene_main_menu_on_enter(void* context) {
    UNUSED(context);
}

bool puff_pacer_scene_main_menu_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void puff_pacer_scene_main_menu_on_exit(void* context) {
    UNUSED(context);
}
```

Create `scenes/puff_pacer_scene_settings.c`:
```c
#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

void puff_pacer_scene_settings_on_enter(void* context) {
    UNUSED(context);
}

bool puff_pacer_scene_settings_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void puff_pacer_scene_settings_on_exit(void* context) {
    UNUSED(context);
}
```

Create `scenes/puff_pacer_scene_session.c`:
```c
#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

void puff_pacer_scene_session_on_enter(void* context) {
    UNUSED(context);
}

bool puff_pacer_scene_session_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void puff_pacer_scene_session_on_exit(void* context) {
    UNUSED(context);
}
```

Create `scenes/puff_pacer_scene_done.c`:
```c
#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

void puff_pacer_scene_done_on_enter(void* context) {
    UNUSED(context);
}

bool puff_pacer_scene_done_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void puff_pacer_scene_done_on_exit(void* context) {
    UNUSED(context);
}
```

Create `scenes/puff_pacer_scene_config.c` (handler tables):
```c
#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

// Forward declarations
#define ADD_SCENE(name, id) \
    void puff_pacer_scene_##name##_on_enter(void*); \
    bool puff_pacer_scene_##name##_on_event(void*, SceneManagerEvent); \
    void puff_pacer_scene_##name##_on_exit(void*);

ADD_SCENE(main_menu, MainMenu)
ADD_SCENE(settings, Settings)
ADD_SCENE(session, Session)
ADD_SCENE(done, Done)

#undef ADD_SCENE

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
```

**Step 7: Verify build**

```bash
ufbt
```
Expected: Build succeeds.

**Step 8: Commit**

```bash
git add -A && git commit -m "feat: app struct, settings persistence, ViewDispatcher/SceneManager skeleton"
```

---

### Task 3: Main Menu scene

**Files:**
- Modify: `scenes/puff_pacer_scene_main_menu.c`

**Step 1: Implement main menu scene**

```c
#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

enum {
    PuffPacerMainMenuIndexStart,
    PuffPacerMainMenuIndexSettings,
};

static void puff_pacer_scene_main_menu_callback(void* context, uint32_t index) {
    PuffPacerApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void puff_pacer_scene_main_menu_on_enter(void* context) {
    PuffPacerApp* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "PuffPacer");
    submenu_add_item(
        app->submenu,
        "Start Session",
        PuffPacerMainMenuIndexStart,
        puff_pacer_scene_main_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Settings",
        PuffPacerMainMenuIndexSettings,
        puff_pacer_scene_main_menu_callback,
        app);
    submenu_set_selected_item(
        app->submenu,
        scene_manager_get_scene_state(app->scene_manager, PuffPacerSceneMainMenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewSubmenu);
}

bool puff_pacer_scene_main_menu_on_event(void* context, SceneManagerEvent event) {
    PuffPacerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, PuffPacerSceneMainMenu, event.event);
        switch(event.event) {
        case PuffPacerMainMenuIndexStart:
            scene_manager_next_scene(app->scene_manager, PuffPacerSceneSession);
            consumed = true;
            break;
        case PuffPacerMainMenuIndexSettings:
            scene_manager_next_scene(app->scene_manager, PuffPacerSceneSettings);
            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        consumed = true;
    }

    return consumed;
}

void puff_pacer_scene_main_menu_on_exit(void* context) {
    PuffPacerApp* app = context;
    submenu_reset(app->submenu);
}
```

**Step 2: Verify build**

```bash
ufbt
```

**Step 3: Commit**

```bash
git add scenes/puff_pacer_scene_main_menu.c && git commit -m "feat: implement main menu scene with start and settings options"
```

---

### Task 4: Settings scene

**Files:**
- Modify: `scenes/puff_pacer_scene_settings.c`

**Step 1: Implement settings scene**

```c
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

    // Interval: 10..40 sec
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
```

**Step 2: Verify build**

```bash
ufbt
```

**Step 3: Commit**

```bash
git add scenes/puff_pacer_scene_settings.c && git commit -m "feat: implement settings scene with puff count and interval controls"
```

---

### Task 5: Session view (custom View with draw + input)

**Files:**
- Create: `views/puff_session_view.h`
- Create: `views/puff_session_view.c`
- Modify: `puff_pacer_app.h` (add session view field)
- Modify: `puff_pacer.c` (alloc/free session view)

**Step 1: Create views/puff_session_view.h**

```c
#pragma once

#include <gui/view.h>

typedef struct {
    uint32_t current_puff;
    uint32_t total_puffs;
    uint32_t seconds_remaining;
    uint32_t total_elapsed;
    bool paused;
} PuffSessionModel;

View* puff_session_view_alloc(void);
void puff_session_view_free(View* view);
View* puff_session_view_get_view(View* view);
```

**Step 2: Create views/puff_session_view.c**

```c
#include "puff_session_view.h"
#include <gui/elements.h>
#include <stdio.h>

static void puff_session_view_draw(Canvas* canvas, void* model) {
    PuffSessionModel* m = model;
    canvas_clear(canvas);

    // Header: "PuffPacer" left, elapsed time right
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, "PuffPacer");

    char time_buf[16];
    uint32_t mins = m->total_elapsed / 60;
    uint32_t secs = m->total_elapsed % 60;
    snprintf(time_buf, sizeof(time_buf), "%02lu:%02lu", (unsigned long)mins, (unsigned long)secs);
    canvas_draw_str_aligned(canvas, 126, 10, AlignRight, AlignBottom, time_buf);

    // Puff counter: "Puff 3 / 10"
    canvas_set_font(canvas, FontPrimary);
    char puff_buf[24];
    snprintf(
        puff_buf,
        sizeof(puff_buf),
        "Puff %lu / %lu",
        (unsigned long)m->current_puff,
        (unsigned long)m->total_puffs);
    canvas_draw_str_aligned(canvas, 64, 28, AlignCenter, AlignBottom, puff_buf);

    // Countdown or PAUSED
    canvas_set_font(canvas, FontSecondary);
    if(m->paused) {
        canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignBottom, "PAUSED");
    } else {
        char next_buf[16];
        snprintf(next_buf, sizeof(next_buf), "Next in: %lus", (unsigned long)m->seconds_remaining);
        canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignBottom, next_buf);
    }

    // Progress bar
    uint8_t bar_x = 14;
    uint8_t bar_y = 44;
    uint8_t bar_w = 100;
    uint8_t bar_h = 8;
    elements_frame(canvas, bar_x, bar_y, bar_w, bar_h);
    if(m->total_puffs > 0) {
        uint8_t fill_w = (uint8_t)((m->current_puff - 1) * (bar_w - 2) / m->total_puffs);
        if(fill_w > 0) {
            canvas_draw_box(canvas, bar_x + 1, bar_y + 1, fill_w, bar_h - 2);
        }
    }

    // Hints
    canvas_set_font(canvas, FontSecondary);
    if(m->paused) {
        canvas_draw_str(canvas, 2, 62, "[OK]=Resume");
    } else {
        canvas_draw_str(canvas, 2, 62, "[OK]=Pause");
    }
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, "[<]=Exit");
}

static bool puff_session_view_input(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    // Input handled via scene_manager custom events in session scene
    return false;
}

View* puff_session_view_alloc(void) {
    View* view = view_alloc();
    view_allocate_model(view, ViewModelTypeLocking, sizeof(PuffSessionModel));
    view_set_draw_callback(view, puff_session_view_draw);
    view_set_input_callback(view, puff_session_view_input);
    return view;
}

void puff_session_view_free(View* view) {
    view_free(view);
}
```

**Step 3: Update puff_pacer_app.h — replace `View* session_view` import**

Add at the top after other includes:
```c
#include "views/puff_session_view.h"
```

The `View* session_view` field in PuffPacerApp remains as-is (it stores the View* returned by puff_session_view_alloc).

**Step 4: Update puff_pacer.c — alloc/free session view**

In `puff_pacer_alloc()`, after widget setup, add:
```c
    // Session view
    app->session_view = puff_session_view_alloc();
    view_set_context(app->session_view, app);
    view_dispatcher_add_view(
        app->view_dispatcher, PuffPacerViewSession, app->session_view);
```

In `puff_pacer_free()`, before widget removal, add:
```c
    view_dispatcher_remove_view(app->view_dispatcher, PuffPacerViewSession);
    puff_session_view_free(app->session_view);
```

**Step 5: Verify build**

```bash
ufbt
```

**Step 6: Commit**

```bash
git add views/ puff_pacer_app.h puff_pacer.c && git commit -m "feat: add custom session view with draw callback for timer display"
```

---

### Task 6: Session scene with timer logic

**Files:**
- Modify: `scenes/puff_pacer_scene_session.c`

**Step 1: Implement session scene**

```c
#include "../puff_pacer_app.h"
#include "../views/puff_session_view.h"
#include "puff_pacer_scene.h"

static void puff_pacer_session_timer_callback(void* context) {
    PuffPacerApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, PuffPacerCustomEventTimerTick);
}

void puff_pacer_scene_session_on_enter(void* context) {
    PuffPacerApp* app = context;

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
        },
        true);

    // First puff notification
    notification_message(app->notifications, &sequence_single_vibro);

    // Start timer
    app->timer =
        furi_timer_alloc(puff_pacer_session_timer_callback, FuriTimerTypePeriodic, app);
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
            with_view_model(
                app->session_view,
                PuffSessionModel * m,
                {
                    m->total_elapsed++;
                    if(m->seconds_remaining > 0) {
                        m->seconds_remaining--;
                    }
                    if(m->seconds_remaining == 0) {
                        if(m->current_puff >= m->total_puffs) {
                            session_done = true;
                        } else {
                            m->current_puff++;
                            m->seconds_remaining = app->settings.interval_sec;
                        }
                    }
                },
                true);

            if(session_done) {
                // Notify completion
                notification_message(app->notifications, &sequence_success);
                scene_manager_next_scene(app->scene_manager, PuffPacerSceneDone);
            } else {
                // Check if we just started a new puff (seconds_remaining was just reset)
                bool new_puff = false;
                with_view_model(
                    app->session_view,
                    PuffSessionModel * m,
                    { new_puff = (m->seconds_remaining == app->settings.interval_sec); },
                    false);
                if(new_puff) {
                    notification_message(app->notifications, &sequence_single_vibro);
                }
            }
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        // Stop timer, go back to menu
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
```

**Step 2: Add input handling in session view**

Update `views/puff_session_view.c` — replace the input callback:

```c
static bool puff_session_view_input(InputEvent* event, void* context) {
    PuffPacerApp* app = context;
    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        // Toggle pause
        bool is_paused = false;
        with_view_model(
            app->session_view,
            PuffSessionModel * m,
            {
                m->paused = !m->paused;
                is_paused = m->paused;
            },
            true);
        if(is_paused) {
            if(app->timer) furi_timer_stop(app->timer);
        } else {
            if(app->timer) furi_timer_start(app->timer, furi_kernel_get_tick_frequency());
        }
        return true;
    }
    return false;
}
```

Add include at top of `views/puff_session_view.c`:
```c
#include "../puff_pacer_app.h"
```

**Step 3: Verify build**

```bash
ufbt
```

**Step 4: Commit**

```bash
git add scenes/puff_pacer_scene_session.c views/puff_session_view.c && git commit -m "feat: implement session scene with timer, pause/resume, and puff notifications"
```

---

### Task 7: Done scene

**Files:**
- Modify: `scenes/puff_pacer_scene_done.c`

**Step 1: Implement done scene**

```c
#include "../puff_pacer_app.h"
#include "puff_pacer_scene.h"

void puff_pacer_scene_done_on_enter(void* context) {
    PuffPacerApp* app = context;

    widget_reset(app->widget);
    widget_add_string_multiline_element(
        app->widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, "Session\nComplete!");
    widget_add_button_element(
        app->widget, GuiButtonTypeCenter, "OK", puff_pacer_scene_done_button_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, PuffPacerViewWidget);
}

static void puff_pacer_scene_done_button_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    PuffPacerApp* app = context;
    if(type == InputTypeShort && result == GuiButtonTypeCenter) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, PuffPacerCustomEventMenuSelected);
    }
}

bool puff_pacer_scene_done_on_event(void* context, SceneManagerEvent event) {
    PuffPacerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, PuffPacerSceneMainMenu);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, PuffPacerSceneMainMenu);
        consumed = true;
    }

    return consumed;
}

void puff_pacer_scene_done_on_exit(void* context) {
    PuffPacerApp* app = context;
    widget_reset(app->widget);
}
```

Note: Move the `static` callback function above `on_enter` so it's declared before use, or add a forward declaration.

**Step 2: Verify build**

```bash
ufbt
```

**Step 3: Commit**

```bash
git add scenes/puff_pacer_scene_done.c && git commit -m "feat: implement done scene showing session complete message"
```

---

### Task 8: Build, lint, and final verification

**Step 1: Run ufbt format**

```bash
ufbt format
```

**Step 2: Run ufbt lint**

```bash
ufbt lint
```
Fix any warnings.

**Step 3: Full clean build**

```bash
ufbt clean && ufbt
```
Expected: Build succeeds with no errors.

**Step 4: Commit any formatting fixes**

```bash
git add -A && git commit -m "chore: format code with ufbt format"
```

---

### Summary of all files

| File | Purpose |
|------|---------|
| `application.fam` | App manifest |
| `icon.png` | 10x10 app icon |
| `puff_pacer.c` | Entry point, alloc/free, ViewDispatcher setup |
| `puff_pacer_app.h` | PuffPacerApp struct, view/scene enums |
| `puff_pacer_settings.h` | Settings struct and constants |
| `puff_pacer_settings.c` | FlipperFormat load/save |
| `views/puff_session_view.h` | Session view model and alloc/free |
| `views/puff_session_view.c` | Custom draw + input for session screen |
| `scenes/puff_pacer_scene.h` | Scene enum + custom events |
| `scenes/puff_pacer_scene_config.c` | Handler tables |
| `scenes/puff_pacer_scene_main_menu.c` | Submenu: Start / Settings |
| `scenes/puff_pacer_scene_settings.c` | VariableItemList: puff count + interval |
| `scenes/puff_pacer_scene_session.c` | Timer logic, notifications |
| `scenes/puff_pacer_scene_done.c` | "Session Complete" widget |
