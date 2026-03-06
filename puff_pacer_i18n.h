#pragma once

#include "puff_pacer_settings.h"

typedef enum {
    PuffPacerTextMenuStart,
    PuffPacerTextMenuSettings,
    PuffPacerTextMenuStatistics,
    PuffPacerTextMenuAbout,

    PuffPacerTextSettingsPuffCount,
    PuffPacerTextSettingsInterval,
    PuffPacerTextSettingsVibration,
    PuffPacerTextSettingsSound,
    PuffPacerTextSettingsLanguage,
    PuffPacerTextSettingsReset,
    PuffPacerTextSettingsResetStats,

    PuffPacerTextCommonOff,
    PuffPacerTextCommonOn,
    PuffPacerTextCommonOk,
    PuffPacerTextCommonYes,
    PuffPacerTextCommonNo,

    PuffPacerTextVibroShort,
    PuffPacerTextVibroLong,

    PuffPacerTextLangEn,
    PuffPacerTextLangRu,

    PuffPacerTextDoneTitle,

    PuffPacerTextResetSettingsTitle,
    PuffPacerTextResetSettingsBody,
    PuffPacerTextResetStatsTitle,
    PuffPacerTextResetStatsBody,

    PuffPacerTextSessionPuffFmt,
    PuffPacerTextSessionPaused,
    PuffPacerTextSessionNextFmt,
    PuffPacerTextSessionPauseHint,
    PuffPacerTextSessionResumeHint,
    PuffPacerTextSessionExitHint,

    PuffPacerTextStatsTitle,
    PuffPacerTextStatsNoCompleted,
    PuffPacerTextStatsFinishOne,
    PuffPacerTextStatsLogFmt,
    PuffPacerTextStatsOkGraph,
    PuffPacerTextStatsScroll,
    PuffPacerTextStatsBack,
    PuffPacerTextStatsList,
    PuffPacerTextStatsDaily,
    PuffPacerTextStatsDoneFmt,
    PuffPacerTextStatsHourTitleFmt,
    PuffPacerTextStatsDays,
    PuffPacerTextStatsHourly,
    PuffPacerTextStatsCountFmt,

    PuffPacerTextAboutLine1,
    PuffPacerTextAboutLine2,
    PuffPacerTextAboutLine3,
    PuffPacerTextAboutLine4,
    PuffPacerTextAboutLine5,
    PuffPacerTextAboutLine6,
    PuffPacerTextAboutLine7,
    PuffPacerTextAboutLine8,
    PuffPacerTextAboutLine9,
    PuffPacerTextAboutLine10,
    PuffPacerTextAboutLine11,

    PuffPacerTextCount,
} PuffPacerTextKey;

const char* puff_pacer_i18n(PuffPacerLanguage language, PuffPacerTextKey key);
