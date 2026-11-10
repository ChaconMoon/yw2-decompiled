/**
 * @file ac.h
 * @brief Nintendo CTR-SDK Auto-Connect Library (`nn::ac`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus

namespace nn {
namespace ac {

    enum Status {
        STATUS_DISCONNECTED = 0,
        STATUS_CONNECTING   = 1,
        STATUS_CONNECTED    = 2
    };

    Result Initialize();
    Result Finalize();

    Result GetStatus(Status* pOutStatus);
    Result GetSignalStrength(u32* pOutStrength);
    Result Connect();
    Result Disconnect();

} // namespace ac
} // namespace nn

#else /* !__cplusplus (Pure C) */

s32  nn_ac_Initialize(void);
void nn_ac_Finalize(void);
s32  nn_ac_GetSignalStrength(u32* pOutStrength);

#endif /* __cplusplus */
