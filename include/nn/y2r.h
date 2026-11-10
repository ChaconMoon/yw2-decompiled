/**
 * @file y2r.h
 * @brief Nintendo CTR-SDK YUV to RGB Conversion Library (`nn::y2r`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus
#include "nn/y2r/CTR/y2r_Y2r.h"
#include "nn/y2r/CTR/detail/detail_Api.h"

namespace nn {
namespace y2r {

    enum InputFormat {
        INPUT_FORMAT_YUV422_INDIV_8 = 0,
        INPUT_FORMAT_YUV420_INDIV_8 = 1,
        INPUT_FORMAT_YUV422_INTERV_8 = 2
    };

    enum OutputFormat {
        OUTPUT_FORMAT_RGBA8_32 = 0,
        OUTPUT_FORMAT_RGB8_24  = 1,
        OUTPUT_FORMAT_RGB565_16= 2,
        OUTPUT_FORMAT_RGB5A1_16= 3,
        OUTPUT_FORMAT_RGBA4_16 = 4
    };

    Result Initialize();
    Result Finalize();
    Result StartConversion();
    Result StopConversion();
    bool   IsBusyConversion();

} // namespace y2r
} // namespace nn

#else /* !__cplusplus (Pure C) */

s32  nn_y2r_Initialize(void);
void nn_y2r_Finalize(void);

#endif /* __cplusplus */
