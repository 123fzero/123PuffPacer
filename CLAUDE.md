# PuffPacer — Flipper Zero App

Interval puff timer for heated tobacco systems (IQOS, Lil Solid 3.0 and similar).

## What It Does
- Configurable puff count (8-20, default 14) and interval (15-40s, default 25s)
- Vibration + beep alert each interval
- Live session screen: puff counter, countdown, elapsed time, progress bar
- Pause/resume with OK button, exit with Back
- Auto-stop after all puffs complete
- Settings persisted to SD card

## Technical Details
- Flipper Zero, SDK (ufbt), API 87.1
- Language: C
- Architecture: ViewDispatcher + SceneManager
- Widgets: Submenu (menu), VariableItemList (settings), custom View (session), Widget (done)
- Timer: FuriTimer periodic 1s
- Notifications: custom sequence (vibro + beep via notification_message)
- Settings: FlipperFormat at /ext/apps_data/puff_pacer/settings.conf
- Category: Tools
- appid: puff_pacer

## Build
```bash
ufbt          # build
ufbt launch   # build + deploy + run
ufbt format   # format code
ufbt lint     # lint code
```

## Communication
- Speak Russian with the developer
- Write code and documentation in English
