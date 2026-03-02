# PuffPacer Design

Interval puff timer for Flipper Zero, designed for heated tobacco systems (Lil Solid 3.0 and similar).

## Requirements

- Configurable puff count (8-20, default 14) and interval (15-40s, default 25s)
- Vibration + beep notification on each puff
- Visual session screen: counter, countdown, elapsed time, progress bar
- Start/pause with OK button, exit with Back
- Auto-stop after all puffs complete
- Settings persisted to SD card between launches

## Architecture

**ViewDispatcher + SceneManager** pattern:
- Native Flipper widgets for menu (`Submenu`) and settings (`VariableItemList`)
- Custom `View` for the session screen only

## File Structure

```
puff_pacer/
├── application.fam
├── puff_pacer.c                  # Entry point, ViewDispatcher/SceneManager init
├── puff_pacer_app.h              # PuffPacerApp struct
├── views/
│   └── puff_session_view.c/.h    # Custom View for session screen
├── scenes/
│   ├── puff_pacer_scene.h        # Scene enum + config tables
│   ├── puff_pacer_scene_main_menu.c
│   ├── puff_pacer_scene_settings.c
│   ├── puff_pacer_scene_session.c
│   └── puff_pacer_scene_done.c
├── puff_pacer_settings.c/.h      # Load/save settings via FlipperFormat
└── icons/
    └── puff_pacer_10px.png
```

## Screens

### 1. Main Menu (Submenu widget)
- "Start Session" → Session scene
- "Settings" → Settings scene

### 2. Settings (VariableItemList widget)
- Puff Count: 8..20 (default 14)
- Interval: 15..40 sec (default 25)
- Changes saved immediately to SD card

### 3. Session (Custom View)
```
┌──────────────────────────┐
│ PuffPacer    02:15       │
│                          │
│     Puff 3 / 14          │
│     Next in: 18s         │
│                          │
│  [███████░░░░░░░]        │
│ [OK]=Pause  [<]=Exit     │
└──────────────────────────┘
```
- OK: toggle pause/resume
- Back: stop session, return to menu
- On pause: "PAUSED" replaces countdown

### 4. Done (Widget or custom draw)
- "Session Complete!"
- OK → return to main menu

## Timer Logic

- `FuriTimer` periodic at 1-second interval
- Each tick: decrement `seconds_remaining`, increment `total_elapsed`
- When `seconds_remaining == 0`: vibration + beep via `notification_message`, increment `current_puff`, reset `seconds_remaining`
- When `current_puff > puff_count`: transition to Done scene
- Pause: stops the timer; resume: restarts it

## Settings Persistence

- Path: `/ext/apps_data/puff_pacer/settings.conf`
- Format: FlipperFormat
- Fields: `PuffCount` (uint32), `IntervalSec` (uint32)
- Load on app start, save on settings change

## Notification

- Vibration + single beep via `notification_message` sequence
- Same notification for every puff interval
