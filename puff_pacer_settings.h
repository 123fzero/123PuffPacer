#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PUFF_COUNT_MIN     8
#define PUFF_COUNT_MAX     20
#define PUFF_COUNT_DEFAULT 10

#define INTERVAL_SEC_MIN     10
#define INTERVAL_SEC_MAX     40
#define INTERVAL_SEC_DEFAULT 30

#define SETTINGS_FILE_PATH    APP_DATA_PATH("settings.conf")
#define SETTINGS_FILE_TYPE    "PuffPacer Settings"
#define SETTINGS_FILE_VERSION 2

typedef enum {
    PuffPacerVibroOff,
    PuffPacerVibroShort,
    PuffPacerVibroLong,
    PuffPacerVibroCount,
} PuffPacerVibroMode;

typedef enum {
    PuffPacerSoundOff,
    PuffPacerSoundOn,
    PuffPacerSoundCount,
} PuffPacerSoundMode;

typedef struct {
    uint32_t puff_count;
    uint32_t interval_sec;
    uint32_t vibro_mode;
    uint32_t sound_mode;
} PuffPacerSettings;

bool puff_pacer_settings_load(PuffPacerSettings* settings);
bool puff_pacer_settings_save(const PuffPacerSettings* settings);

#define PUFF_PACER_STATS_MAX_LOGS 24

#define STATS_FILE_PATH    APP_DATA_PATH("stats.conf")
#define STATS_FILE_TYPE    "PuffPacer Stats"
#define STATS_FILE_VERSION 2

typedef struct {
    uint32_t date_key;
    uint32_t time_key;
    uint32_t puff_count;
    uint32_t interval_sec;
} PuffPacerStatsEntry;

typedef struct {
    uint32_t started_count;
    uint32_t completed_count;
    uint32_t log_count;
    uint32_t next_index;
    PuffPacerStatsEntry logs[PUFF_PACER_STATS_MAX_LOGS];
} PuffPacerStats;

bool puff_pacer_stats_load(PuffPacerStats* stats);
bool puff_pacer_stats_save(const PuffPacerStats* stats);
void puff_pacer_stats_log_started(PuffPacerStats* stats);
void puff_pacer_stats_log_completed(
    PuffPacerStats* stats,
    uint32_t date_key,
    uint32_t time_key,
    uint32_t puff_count,
    uint32_t interval_sec);
void puff_pacer_stats_reset(PuffPacerStats* stats);
