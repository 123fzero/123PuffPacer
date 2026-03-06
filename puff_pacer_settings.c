#include "puff_pacer_settings.h"
#include <furi.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>
#include <string.h>

bool puff_pacer_settings_save(const PuffPacerSettings* settings) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = false;

    do {
        if(!flipper_format_file_open_always(ff, SETTINGS_FILE_PATH)) break;
        if(!flipper_format_write_header_cstr(ff, SETTINGS_FILE_TYPE, SETTINGS_FILE_VERSION)) break;
        if(!flipper_format_write_uint32(ff, "PuffCount", &settings->puff_count, 1)) break;
        if(!flipper_format_write_uint32(ff, "IntervalSec", &settings->interval_sec, 1)) break;
        if(!flipper_format_write_uint32(ff, "VibroMode", &settings->vibro_mode, 1)) break;
        if(!flipper_format_write_uint32(ff, "SoundMode", &settings->sound_mode, 1)) break;
        if(!flipper_format_write_uint32(ff, "Language", &settings->language, 1)) break;
        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool puff_pacer_settings_load(PuffPacerSettings* settings) {
    settings->puff_count = PUFF_COUNT_DEFAULT;
    settings->interval_sec = INTERVAL_SEC_DEFAULT;
    settings->vibro_mode = PuffPacerVibroShort;
    settings->sound_mode = PuffPacerSoundOn;
    settings->language = PuffPacerLanguageEn;

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
        flipper_format_read_uint32(ff, "VibroMode", &settings->vibro_mode, 1);
        flipper_format_read_uint32(ff, "SoundMode", &settings->sound_mode, 1);
        flipper_format_read_uint32(ff, "Language", &settings->language, 1);

        // Clamp to valid ranges
        if(settings->puff_count < PUFF_COUNT_MIN || settings->puff_count > PUFF_COUNT_MAX) {
            settings->puff_count = PUFF_COUNT_DEFAULT;
        }
        if(settings->interval_sec < INTERVAL_SEC_MIN ||
           settings->interval_sec > INTERVAL_SEC_MAX) {
            settings->interval_sec = INTERVAL_SEC_DEFAULT;
        }
        if(settings->vibro_mode >= PuffPacerVibroCount) {
            settings->vibro_mode = PuffPacerVibroShort;
        }
        if(settings->sound_mode >= PuffPacerSoundCount) {
            settings->sound_mode = PuffPacerSoundOn;
        }
        if(settings->language >= PuffPacerLanguageCount) {
            settings->language = PuffPacerLanguageEn;
        }

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool puff_pacer_stats_save(const PuffPacerStats* stats) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = false;
    uint32_t dates[PUFF_PACER_STATS_MAX_LOGS];
    uint32_t times[PUFF_PACER_STATS_MAX_LOGS];
    uint32_t puff_counts[PUFF_PACER_STATS_MAX_LOGS];
    uint32_t intervals[PUFF_PACER_STATS_MAX_LOGS];

    for(uint32_t i = 0; i < PUFF_PACER_STATS_MAX_LOGS; i++) {
        dates[i] = stats->logs[i].date_key;
        times[i] = stats->logs[i].time_key;
        puff_counts[i] = stats->logs[i].puff_count;
        intervals[i] = stats->logs[i].interval_sec;
    }

    do {
        if(!flipper_format_file_open_always(ff, STATS_FILE_PATH)) break;
        if(!flipper_format_write_header_cstr(ff, STATS_FILE_TYPE, STATS_FILE_VERSION)) break;
        if(!flipper_format_write_uint32(ff, "StartedCount", &stats->started_count, 1)) break;
        if(!flipper_format_write_uint32(ff, "CompletedCount", &stats->completed_count, 1)) break;
        if(!flipper_format_write_uint32(ff, "LogCount", &stats->log_count, 1)) break;
        if(!flipper_format_write_uint32(ff, "NextIndex", &stats->next_index, 1)) break;
        if(!flipper_format_write_uint32(ff, "LogDates", dates, PUFF_PACER_STATS_MAX_LOGS)) break;
        if(!flipper_format_write_uint32(ff, "LogTimes", times, PUFF_PACER_STATS_MAX_LOGS)) break;
        if(!flipper_format_write_uint32(
               ff, "LogPuffCounts", puff_counts, PUFF_PACER_STATS_MAX_LOGS))
            break;
        if(!flipper_format_write_uint32(ff, "LogIntervals", intervals, PUFF_PACER_STATS_MAX_LOGS))
            break;
        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool puff_pacer_stats_load(PuffPacerStats* stats) {
    memset(stats, 0, sizeof(PuffPacerStats));

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = false;

    do {
        if(!flipper_format_file_open_existing(ff, STATS_FILE_PATH)) break;

        FuriString* filetype = furi_string_alloc();
        uint32_t version = 0;
        if(!flipper_format_read_header(ff, filetype, &version)) {
            furi_string_free(filetype);
            break;
        }
        if(furi_string_cmp_str(filetype, STATS_FILE_TYPE) != 0 || version != STATS_FILE_VERSION) {
            furi_string_free(filetype);
            break;
        }
        furi_string_free(filetype);

        uint32_t dates[PUFF_PACER_STATS_MAX_LOGS] = {0};
        uint32_t times[PUFF_PACER_STATS_MAX_LOGS] = {0};
        uint32_t puff_counts[PUFF_PACER_STATS_MAX_LOGS] = {0};
        uint32_t intervals[PUFF_PACER_STATS_MAX_LOGS] = {0};

        flipper_format_read_uint32(ff, "StartedCount", &stats->started_count, 1);
        flipper_format_read_uint32(ff, "CompletedCount", &stats->completed_count, 1);
        flipper_format_read_uint32(ff, "LogCount", &stats->log_count, 1);
        flipper_format_read_uint32(ff, "NextIndex", &stats->next_index, 1);
        flipper_format_read_uint32(ff, "LogDates", dates, PUFF_PACER_STATS_MAX_LOGS);
        flipper_format_read_uint32(ff, "LogTimes", times, PUFF_PACER_STATS_MAX_LOGS);
        flipper_format_read_uint32(ff, "LogPuffCounts", puff_counts, PUFF_PACER_STATS_MAX_LOGS);
        flipper_format_read_uint32(ff, "LogIntervals", intervals, PUFF_PACER_STATS_MAX_LOGS);

        if(stats->log_count > PUFF_PACER_STATS_MAX_LOGS) {
            stats->log_count = PUFF_PACER_STATS_MAX_LOGS;
        }
        if(stats->next_index >= PUFF_PACER_STATS_MAX_LOGS) {
            stats->next_index = stats->log_count % PUFF_PACER_STATS_MAX_LOGS;
        }

        for(uint32_t i = 0; i < PUFF_PACER_STATS_MAX_LOGS; i++) {
            stats->logs[i].date_key = dates[i];
            stats->logs[i].time_key = times[i];
            stats->logs[i].puff_count = puff_counts[i];
            stats->logs[i].interval_sec = intervals[i];
        }

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

void puff_pacer_stats_log_started(PuffPacerStats* stats) {
    stats->started_count++;
}

void puff_pacer_stats_log_completed(
    PuffPacerStats* stats,
    uint32_t date_key,
    uint32_t time_key,
    uint32_t puff_count,
    uint32_t interval_sec) {
    stats->completed_count++;
    stats->logs[stats->next_index].date_key = date_key;
    stats->logs[stats->next_index].time_key = time_key;
    stats->logs[stats->next_index].puff_count = puff_count;
    stats->logs[stats->next_index].interval_sec = interval_sec;
    stats->next_index = (stats->next_index + 1) % PUFF_PACER_STATS_MAX_LOGS;
    if(stats->log_count < PUFF_PACER_STATS_MAX_LOGS) {
        stats->log_count++;
    }
}

void puff_pacer_stats_reset(PuffPacerStats* stats) {
    memset(stats, 0, sizeof(PuffPacerStats));
}
