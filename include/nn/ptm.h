/**
 * @file ptm.h
 * @brief Nintendo CTR-SDK Power and Thermal Management Library (`nn::ptm`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus

namespace nn {
namespace ptm {

    enum BatteryLevel {
        BATTERY_LEVEL_EMPTY    = 0,
        BATTERY_LEVEL_LOW      = 1,
        BATTERY_LEVEL_MED_LOW  = 2,
        BATTERY_LEVEL_MED_HIGH = 3,
        BATTERY_LEVEL_FULL     = 4,
        BATTERY_LEVEL_CHARGING = 5
    };

    enum BatteryChargeState {
        CHARGE_STATE_NOT_CHARGING = 0,
        CHARGE_STATE_CHARGING     = 1
    };

    enum ShellStatus {
        SHELL_OPEN   = 0,
        SHELL_CLOSED = 1
    };

    Result Initialize();
    Result Finalize();

    Result GetBatteryLevel(u32* pOutLevel);
    Result GetBatteryChargeState(u32* pOutState);
    Result GetPedometerStepCount(u32* pOutSteps);
    Result GetShellStatus(u32* pOutStatus);

} // namespace ptm
} // namespace nn

#else /* !__cplusplus (Pure C) */

s32 nn_ptm_Initialize(void);
void nn_ptm_Finalize(void);
s32 nn_ptm_GetBatteryLevel(u32* pOutLevel);
s32 nn_ptm_GetPedometerStepCount(u32* pOutSteps);

#endif /* __cplusplus */
