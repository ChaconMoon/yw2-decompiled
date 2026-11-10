/**
 * @file ndm.h
 * @brief Nintendo CTR-SDK Network Daemon Manager Library (`nn::ndm`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"
#include "nn/ndm/ndm_Api.h"

#ifdef __cplusplus

namespace nn {
namespace ndm {

    enum DaemonType {
        DAEMON_CEC     = (1 << 0),
        DAEMON_BOSS    = (1 << 1),
        DAEMON_NIM     = (1 << 2),
        DAEMON_FRIENDS = (1 << 3)
    };

    Result Initialize();
    Result Finalize();
    Result SuspendDaemons(u32 daemonMask);
    Result ResumeDaemons(u32 daemonMask);

} // namespace ndm
} // namespace nn

#else /* !__cplusplus (Pure C) */

s32  nn_ndm_Initialize(void);
void nn_ndm_Finalize(void);
s32  nn_ndm_SuspendDaemons(u32 daemonMask);
s32  nn_ndm_ResumeDaemons(u32 daemonMask);

#endif /* __cplusplus */
