# 123PuffPacer — Flipper Zero App

Interval puff timer for heated tobacco systems (IQOS, Lil Solid 3.0 and similar).

## What It Does
- Configurable puff count (8-20, default 12) and interval (10-40s, default 25s)
- Vibration control: Off / Short / Long
- Sound control: Off / On
- Smoke animation on each puff (5 seconds, procedural S-wisps on both sides)
- Live session screen: puff counter, countdown, elapsed time, progress bar
- Reset Settings option in settings menu
- Pause/resume with OK button, exit with Back
- Auto-stop after all puffs complete
- Settings persisted to SD card
- About screen with author and GitHub link

## Technical Details
- Flipper Zero, SDK (ufbt), API 87.1
- Firmware tested: Momentum mntm-012
- Language: C
- Architecture: ViewDispatcher + SceneManager
- 5 scenes: MainMenu, Settings, Session, Done, About
- Widgets: Submenu (menu), VariableItemList (settings), custom View (session), Widget (done/about)
- Timer: FuriTimer periodic 1s
- Notifications: separate vibro and sound sequences, configurable
- Settings: FlipperFormat v2 at /ext/apps_data/puff_pacer/settings.conf
- Category: Tools
- appid: puff_pacer
- Icon: 10x10 smoke cloud
- Versioning: semver (tag + GitHub release)

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
