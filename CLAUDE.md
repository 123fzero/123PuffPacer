# 123PuffPacer — Flipper Zero App

Interval puff timer for heated tobacco systems (IQOS, Lil Solid 3.0 and similar).

## What It Does
- Configurable puff count (8-20, default 10) and interval (10-40s, default 30s)
- Vibration control: Off / Short / Long
- Sound control: Off / On
- Smoke animation on each puff (5 seconds, procedural S-wisps on both sides)
- Live session screen: puff counter, countdown, elapsed time, progress bar
- Statistics: started/completed counters + circular session log (24 entries)
- Log entries include date, time, puff count, and interval
- Daily graph (7 days) + hourly drill-down for selected day
- Reset Statistics option with confirmation dialog
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
- 7 scenes: MainMenu, Settings, Stats, ResetStatsConfirm, Session, Done, About
- Views: Submenu (menu), VariableItemList (settings), custom View (session), custom View (stats), Widget (done/about/confirm)
- Timer: FuriTimer periodic 1s
- Notifications: separate vibro and sound sequences, configurable
- Settings: FlipperFormat v2 at /ext/apps_data/puff_pacer/settings.conf
- Stats: FlipperFormat v2 at /ext/apps_data/puff_pacer/stats.conf
- Category: Tools
- appid: puff_pacer
- Icon: 10x10 smoke cloud
- Versioning: semver (tag + GitHub release)
- Single source of truth for app version: `puff_pacer_version.h`
- About screen version must use `PUFF_PACER_VERSION_STR` from `puff_pacer_version.h`
- Before creating a release: bump `puff_pacer_version.h`, commit, tag with the same semver, push, then create GitHub release

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
