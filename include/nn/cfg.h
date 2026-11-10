/**
 * @file cfg.h
 * @brief Nintendo CTR-SDK Configuration Library (`nn::cfg`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus
#include "nn/cfg/CTR/cfg_Api.h"

namespace nn {
namespace cfg {

    enum SystemLanguage {
        LANGUAGE_JA = 0,
        LANGUAGE_EN = 1,
        LANGUAGE_FR = 2,
        LANGUAGE_DE = 3,
        LANGUAGE_IT = 4,
        LANGUAGE_ES = 5,
        LANGUAGE_ZH = 6,
        LANGUAGE_KO = 7,
        LANGUAGE_NL = 8,
        LANGUAGE_PT = 9,
        LANGUAGE_RU = 10,
        LANGUAGE_TW = 11
    };

    enum SystemRegion {
        REGION_JP  = 0,
        REGION_US  = 1,
        REGION_EU  = 2,
        REGION_AUS = 3,
        REGION_CHN = 4,
        REGION_KOR = 5,
        REGION_TWN = 6
    };

    enum SoundOutputMode {
        SOUND_MODE_MONO     = 0,
        SOUND_MODE_STEREO   = 1,
        SOUND_MODE_SURROUND = 2
    };

    SystemLanguage  GetSystemLanguage();
    SystemRegion    GetRegion();
    SoundOutputMode GetSoundOutputMode();
    Result          GetUserName(u16* pOutName, u32 maxChars);

    namespace CTR {
        using ::nn::cfg::CTR::Initialize;
    } // namespace CTR

} // namespace cfg
} // namespace nn

#else /* !__cplusplus (Pure C) */

u8   nn_cfg_GetSystemLanguage(void);
u8   nn_cfg_GetRegion(void);
u8   nn_cfg_GetSoundOutputMode(void);

#endif /* __cplusplus */
