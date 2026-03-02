#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PUFF_COUNT_MIN     8
#define PUFF_COUNT_MAX     20
#define PUFF_COUNT_DEFAULT 14

#define INTERVAL_SEC_MIN     15
#define INTERVAL_SEC_MAX     40
#define INTERVAL_SEC_DEFAULT 25

#define SETTINGS_FILE_PATH    APP_DATA_PATH("settings.conf")
#define SETTINGS_FILE_TYPE    "PuffPacer Settings"
#define SETTINGS_FILE_VERSION 1

typedef struct {
    uint32_t puff_count;
    uint32_t interval_sec;
} PuffPacerSettings;

bool puff_pacer_settings_load(PuffPacerSettings* settings);
bool puff_pacer_settings_save(const PuffPacerSettings* settings);
