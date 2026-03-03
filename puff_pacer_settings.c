#include "puff_pacer_settings.h"
#include <furi.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

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

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}
