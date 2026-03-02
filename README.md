# PuffPacer — Puff Interval Timer for Flipper Zero

Interval puff timer for heated tobacco devices (IQOS, Lil Solid, glo, Ploom, etc.). Helps you pace your puffs evenly throughout a session with vibration and sound alerts.

## Why?

Heated tobacco sticks have a limited number of puffs and a fixed session time. If you puff too fast, you waste the stick. PuffPacer divides your session into equal intervals and alerts you when it's time for the next puff — so you get the most out of every stick.

## Features

- **Configurable puff count** — 8 to 20 puffs per session (default: 14)
- **Configurable interval** — 15 to 40 seconds between puffs (default: 25s)
- **Vibration control** — Off / Short / Long
- **Sound control** — Off / On
- **Live session screen** — puff counter, countdown timer, elapsed time, progress bar
- **Pause/resume** with OK button
- **Persistent settings** — saved to SD card, remembered between launches
- **Backlight stays on** during active session
- **About screen** with version info and links

## Screenshots

```
┌────────────────────────────┐   ┌────────────────────────────┐
│      PuffPacer             │   │ Puff Count          < 14 > │
│                            │   │ Interval            < 25s >│
│  > Start Session           │   │ Vibration         < Short >│
│    Settings                │   │ Sound                < On >│
│    About                   │   │                            │
│                            │   │                            │
│                            │   │                            │
└────────────────────────────┘   └────────────────────────────┘
         Main Menu                         Settings

┌────────────────────────────┐   ┌────────────────────────────┐
│ PuffPacer          02:15   │   │                            │
│                            │   │                            │
│       Puff 3 / 14          │   │         Session            │
│       Next in: 18s         │   │        Complete!            │
│                            │   │                            │
│    [███████░░░░░░░]        │   │           [OK]              │
│ [OK]=Pause    [<]=Exit     │   │                            │
└────────────────────────────┘   └────────────────────────────┘
        Session Screen                   Done Screen
```

## Compatible Devices

Works with any heated tobacco / heat-not-burn device:

- **IQOS** — ILUMA, ILUMA Prime, ILUMA One, DUO, 3
- **Lil** — Solid 3.0, Solid 2.0, Solid EZ, Hybrid 2.0
- **glo** — Hyper X2, Hyper+, Pro, Nano
- **Ploom** — X Advanced, X, S
- **Jouz** — 20, 20S, 12
- And any similar device with timed sessions

## Install

### From .fap file

1. Download `puff_pacer.fap` from [Releases](../../releases)
2. Copy to your Flipper Zero SD card: `SD Card/apps/Tools/`
3. Open on Flipper: `Applications → Tools → PuffPacer`

### Build from source

```bash
# Install ufbt if you haven't
pip install ufbt

# Clone and build
git clone https://github.com/123fzero/123PuffPacer.git
cd 123PuffPacer
ufbt

# Build and launch on connected Flipper
ufbt launch
```

## Usage

1. Open PuffPacer from Applications → Tools
2. (Optional) Go to **Settings** to adjust puff count, interval, vibration, and sound
3. Select **Start Session**
4. Flipper alerts you on each interval — take a puff!
5. Press **OK** to pause/resume, **Back** to exit
6. After all puffs — "Session Complete!" screen

## Controls

| Button | Action |
|--------|--------|
| OK | Start session / Pause / Resume |
| Back | Go back / Exit session |
| Left/Right | Change settings values |

## Tested On

- **Firmware:** Momentum (mntm-012)
- **API:** 87.1
- **Hardware:** Flipper Zero (f7)

Should also work on official firmware and other custom firmwares (Unleashed, RogueMaster, Xtreme) with compatible API versions.

## License

MIT
