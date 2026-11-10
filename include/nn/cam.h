/**
 * @file cam.h
 * @brief Nintendo CTR-SDK Camera Library (`nn::cam`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus

namespace nn {
namespace cam {

    enum CameraPort {
        PORT_CAM1 = 0,
        PORT_CAM2 = 1
    };

    enum CameraSelect {
        SELECT_IN1       = (1 << 0),
        SELECT_OUT1      = (1 << 1),
        SELECT_OUT2      = (1 << 2),
        SELECT_OUT1_OUT2 = (SELECT_OUT1 | SELECT_OUT2)
    };

    enum Resolution {
        RESOLUTION_VGA   = 0,
        RESOLUTION_QVGA  = 1,
        RESOLUTION_QQVGA = 2,
        RESOLUTION_CTR   = 3
    };

    Result Initialize();
    Result Finalize();
    Result StartCapture(CameraPort port);
    Result StopCapture(CameraPort port);
    Result SetStereoCameraMode(bool isStereo);

} // namespace cam
} // namespace nn

#else /* !__cplusplus (Pure C) */

s32  nn_cam_Initialize(void);
void nn_cam_Finalize(void);

#endif /* __cplusplus */
