/**
 * @file err.h
 * @brief Nintendo CTR-SDK Error Display Library (`nn::err`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus
#include "nn/err/CTR/err_Api.h"

namespace nn {
namespace err {

    using CTR::ThrowFatalErr;

    void ThrowFatal(Result result);
    void ShowError(Result result);

} // namespace err
} // namespace nn

#else /* !__cplusplus (Pure C) */

void nn_err_ThrowFatal(s32 result);

#endif /* __cplusplus */
