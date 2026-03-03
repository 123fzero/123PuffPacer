# Smoke Animation Design

## Overview
Add procedural smoke animation to session view. Wavy S-shaped smoke wisps appear on both sides of the screen for 5 seconds at the start of each puff.

## Changes

### Model (`puff_session_view.h`)
- `smoke_timer` (uint8_t): countdown 5→0, smoke visible when >0
- `smoke_frame` (uint8_t): animation frame 0-4, incremented each second

### Rendering (`puff_session_view.c`)
- When `smoke_timer > 0`, draw S-shaped wisps on left (x: 0-12) and right (x: 116-127)
- Each wisp: small curve of 4-6 pixels
- ~6-8 wisps per side at varying heights
- `smoke_frame` shifts positions upward — rising smoke effect
- Wisps that exit top of screen disappear

### Session logic (`puff_pacer_scene_session.c`)
- On `on_enter` (first puff): set `smoke_timer = 5`
- On `new_puff`: set `smoke_timer = 5`
- On timer tick: if `smoke_timer > 0`, decrement and increment `smoke_frame`
- Paused: smoke freezes (no tick)

### Settings (`puff_pacer_settings.h`)
- `INTERVAL_SEC_MIN`: 15 → 10

## Not changed
- Center UI layout (text, progress bar, hints)
- No new files
