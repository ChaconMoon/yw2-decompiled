/**
 * @file mic.h
 * @brief Nintendo CTR-SDK Microphone Library (`nn::mic`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus

namespace nn {
namespace mic {

    enum SamplingRate {
        RATE_8140_HZ  = 0,
        RATE_10850_HZ = 1,
        RATE_16270_HZ = 2,
        RATE_32560_HZ = 3
    };

    Result Initialize();
    Result Finalize();
    Result StartRecording(void* buffer, u32 bufferSize, SamplingRate rate);
    Result StopRecording();
    Result ReadAudioData(void* buffer, u32 size, u32* pOutReadSize);

} // namespace mic
} // namespace nn

#else /* !__cplusplus (Pure C) */

s32  nn_mic_Initialize(void);
void nn_mic_Finalize(void);

#endif /* __cplusplus */
