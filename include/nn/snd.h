/**
 * @file snd.h
 * @brief Nintendo CTR-SDK Sound Library (`nn::snd`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Handle.h"
#include "nn/Result.h"
#include "nn/snd/CTR/snd_Dspsnd.h"

#ifdef __cplusplus

namespace nn {
namespace snd {

    enum SoundFormat {
        SOUND_FORMAT_PCM8  = 0,
        SOUND_FORMAT_PCM16 = 1,
        SOUND_FORMAT_ADPCM = 2
    };

    struct WaveBuffer {
        const void* buffer;
        u32         size;
        u32         loopStart;
        u32         loopEnd;
        u8          format;
        bool        isLooping;
    };

    Result Initialize();
    Result Finalize();
    void   SetMasterVolume(float volume);
    float  GetMasterVolume();

    namespace CTR {
        Result Initialize();
        Result Finalize();
    } // namespace CTR

} // namespace snd
} // namespace nn

#else /* !__cplusplus (Pure C) */

typedef struct nn_snd_WaveBuffer {
    const void* buffer;
    u32         size;
    u32         loopStart;
    u32         loopEnd;
    u8          format;
    bool32      isLooping;
} nn_snd_WaveBuffer;

s32   nn_snd_Initialize(void);
void  nn_snd_Finalize(void);
void  nn_snd_SetMasterVolume(float volume);
float nn_snd_GetMasterVolume(void);

#endif /* __cplusplus */
