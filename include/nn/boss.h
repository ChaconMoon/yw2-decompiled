/**
 * @file boss.h
 * @brief Nintendo CTR-SDK SpotPass Library (`nn::boss`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus

namespace nn {
namespace boss {

    enum TaskStatus {
        TASK_STATUS_NOT_REGISTERED = 0,
        TASK_STATUS_WAITING        = 1,
        TASK_STATUS_RUNNING        = 2,
        TASK_STATUS_SUCCESS        = 3,
        TASK_STATUS_ERROR          = 4
    };

    Result Initialize();
    Result Finalize();
    Result RegisterTask(const char* taskName, u32 priority);
    Result UnregisterTask(const char* taskName);
    Result GetTaskStatus(const char* taskName, TaskStatus* pOutStatus);

} // namespace boss
} // namespace nn

#else /* !__cplusplus (Pure C) */

s32  nn_boss_Initialize(void);
void nn_boss_Finalize(void);

#endif /* __cplusplus */
