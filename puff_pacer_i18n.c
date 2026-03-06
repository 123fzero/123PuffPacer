#include "puff_pacer_i18n.h"

static const char* const puff_pacer_text_en[PuffPacerTextCount] = {
    [PuffPacerTextMenuStart] = "Start Session",
    [PuffPacerTextMenuSettings] = "Settings",
    [PuffPacerTextMenuStatistics] = "Statistics",
    [PuffPacerTextMenuAbout] = "About",

    [PuffPacerTextSettingsPuffCount] = "Puff Count",
    [PuffPacerTextSettingsInterval] = "Interval",
    [PuffPacerTextSettingsVibration] = "Vibration",
    [PuffPacerTextSettingsSound] = "Sound",
    [PuffPacerTextSettingsLanguage] = "Language",
    [PuffPacerTextSettingsReset] = "Reset Settings",
    [PuffPacerTextSettingsResetStats] = "Reset Statistics",

    [PuffPacerTextCommonOff] = "Off",
    [PuffPacerTextCommonOn] = "On",
    [PuffPacerTextCommonOk] = "OK",
    [PuffPacerTextCommonYes] = "Yes",
    [PuffPacerTextCommonNo] = "No",

    [PuffPacerTextVibroShort] = "Short",
    [PuffPacerTextVibroLong] = "Long",

    [PuffPacerTextLangEn] = "EN",
    [PuffPacerTextLangRu] = "RU",

    [PuffPacerTextDoneTitle] = "Session\nComplete!",

    [PuffPacerTextResetSettingsTitle] = "Reset\nsettings?",
    [PuffPacerTextResetSettingsBody] = "Puffs, interval,\nalerts, lang default",
    [PuffPacerTextResetStatsTitle] = "Reset\nstatistics?",
    [PuffPacerTextResetStatsBody] = "This will erase\nall session logs",

    [PuffPacerTextSessionPuffFmt] = "Puff %lu / %lu",
    [PuffPacerTextSessionPaused] = "PAUSED",
    [PuffPacerTextSessionNextFmt] = "Next in: %lus",
    [PuffPacerTextSessionPauseHint] = "[OK]=Pause",
    [PuffPacerTextSessionResumeHint] = "[OK]=Resume",
    [PuffPacerTextSessionExitHint] = "[<]=Exit",

    [PuffPacerTextStatsTitle] = "Statistics",
    [PuffPacerTextStatsNoCompleted] = "No completed",
    [PuffPacerTextStatsFinishOne] = "Finish one to log",
    [PuffPacerTextStatsLogFmt] = "Log %u-%u/%lu",
    [PuffPacerTextStatsOkGraph] = "OK Graph",
    [PuffPacerTextStatsScroll] = "^v Scroll",
    [PuffPacerTextStatsBack] = "< Back",
    [PuffPacerTextStatsList] = "< List",
    [PuffPacerTextStatsDaily] = "Daily graph",
    [PuffPacerTextStatsDoneFmt] = "D:%lu",
    [PuffPacerTextStatsHourTitleFmt] = "%02lu.%02lu by hour",
    [PuffPacerTextStatsDays] = "< Days",
    [PuffPacerTextStatsHourly] = "Hourly",
    [PuffPacerTextStatsCountFmt] = "Count:%u",

    [PuffPacerTextAboutLine1] = "Flipper Zero",
    [PuffPacerTextAboutLine2] = "puff timer",
    [PuffPacerTextAboutLine3] = "for heated tobacco",
    [PuffPacerTextAboutLine4] = "IQOS, HEETS, TEREA,",
    [PuffPacerTextAboutLine5] = "Lil Solid, glo, Ploom",
    [PuffPacerTextAboutLine6] = "Like a metronome",
    [PuffPacerTextAboutLine7] = "for smokers",
    [PuffPacerTextAboutLine8] = "Advisory use only",
    [PuffPacerTextAboutLine9] = "Author: 123fzero",
    [PuffPacerTextAboutLine10] = "github.com/123fzero",
    [PuffPacerTextAboutLine11] = "/123PuffPacer",
};

static const char* const puff_pacer_text_ru[PuffPacerTextCount] = {
    [PuffPacerTextMenuStart] = "Start seans",
    [PuffPacerTextMenuSettings] = "Nastroiki",
    [PuffPacerTextMenuStatistics] = "Statistika",
    [PuffPacerTextMenuAbout] = "O prilozhenii",

    [PuffPacerTextSettingsPuffCount] = "Kol-vo tyag",
    [PuffPacerTextSettingsInterval] = "Interval",
    [PuffPacerTextSettingsVibration] = "Vibraciya",
    [PuffPacerTextSettingsSound] = "Zvuk",
    [PuffPacerTextSettingsLanguage] = "Yazyk",
    [PuffPacerTextSettingsReset] = "Sbros nastroek",
    [PuffPacerTextSettingsResetStats] = "Sbros statistiki",

    [PuffPacerTextCommonOff] = "Vykl",
    [PuffPacerTextCommonOn] = "Vkl",
    [PuffPacerTextCommonOk] = "OK",
    [PuffPacerTextCommonYes] = "Da",
    [PuffPacerTextCommonNo] = "Net",

    [PuffPacerTextVibroShort] = "Korotko",
    [PuffPacerTextVibroLong] = "Dlinno",

    [PuffPacerTextLangEn] = "EN",
    [PuffPacerTextLangRu] = "RU",

    [PuffPacerTextDoneTitle] = "Seans\nzavershen!",

    [PuffPacerTextResetSettingsTitle] = "Sbros\nnastroek?",
    [PuffPacerTextResetSettingsBody] = "Tyagi, interval,\nalerty, yazyk",
    [PuffPacerTextResetStatsTitle] = "Sbros\nstatistiki?",
    [PuffPacerTextResetStatsBody] = "Eto udalit\nvse logi seansov",

    [PuffPacerTextSessionPuffFmt] = "Tyaga %lu / %lu",
    [PuffPacerTextSessionPaused] = "PAUZA",
    [PuffPacerTextSessionNextFmt] = "Sleduyushchaya: %lus",
    [PuffPacerTextSessionPauseHint] = "[OK]=Pauza",
    [PuffPacerTextSessionResumeHint] = "[OK]=Prodolzhit",
    [PuffPacerTextSessionExitHint] = "[<]=Vyhod",

    [PuffPacerTextStatsTitle] = "Statistika",
    [PuffPacerTextStatsNoCompleted] = "Zavershennyh net",
    [PuffPacerTextStatsFinishOne] = "Zavershi odin seans",
    [PuffPacerTextStatsLogFmt] = "Log %u-%u/%lu",
    [PuffPacerTextStatsOkGraph] = "OK Grafik",
    [PuffPacerTextStatsScroll] = "^v Prokrutka",
    [PuffPacerTextStatsBack] = "< Nazad",
    [PuffPacerTextStatsList] = "< Spisok",
    [PuffPacerTextStatsDaily] = "Grafik po dnyam",
    [PuffPacerTextStatsDoneFmt] = "Z:%lu",
    [PuffPacerTextStatsHourTitleFmt] = "%02lu.%02lu po chasam",
    [PuffPacerTextStatsDays] = "< Dni",
    [PuffPacerTextStatsHourly] = "Po chasam",
    [PuffPacerTextStatsCountFmt] = "Kol-vo:%u",

    [PuffPacerTextAboutLine1] = "Flipper Zero",
    [PuffPacerTextAboutLine2] = "timer tyag",
    [PuffPacerTextAboutLine3] = "dlya nagreva tabaka",
    [PuffPacerTextAboutLine4] = "IQOS, HEETS, TEREA,",
    [PuffPacerTextAboutLine5] = "Lil Solid, glo, Ploom",
    [PuffPacerTextAboutLine6] = "Kak metronom",
    [PuffPacerTextAboutLine7] = "dlya kurilshchika",
    [PuffPacerTextAboutLine8] = "Tolko rekomendacii",
    [PuffPacerTextAboutLine9] = "Author: 123fzero",
    [PuffPacerTextAboutLine10] = "github.com/123fzero",
    [PuffPacerTextAboutLine11] = "/123PuffPacer",
};

const char* puff_pacer_i18n(PuffPacerLanguage language, PuffPacerTextKey key) {
    if(key >= PuffPacerTextCount) {
        return "";
    }

    const char* const* table = puff_pacer_text_en;
    if(language == PuffPacerLanguageRu) {
        table = puff_pacer_text_ru;
    }

    const char* value = table[key];
    return value ? value : puff_pacer_text_en[key];
}
