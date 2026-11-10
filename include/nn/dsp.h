/**
 * @file dsp.h
 * @brief Nintendo CTR-SDK Digital Signal Processor Library (`nn::dsp`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Handle.h"
#include "nn/Result.h"

#ifdef __cplusplus
#include "nn/dsp/CTR/CTR_Api.h"
#include "nn/dsp/CTR/dsp_DSP.h"

namespace nn {
namespace dsp {

    /* High-level DSP aliases forwarding from nn::dsp::CTR */
    using CTR::Initialize;
    using CTR::Finalize;
    using CTR::LoadDefaultComponent;
    using CTR::LoadComponent;
    using CTR::UnloadComponent;
    using CTR::IsComponentLoaded;

} // namespace dsp
} // namespace nn

#endif /* __cplusplus */
