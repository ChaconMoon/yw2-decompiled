/**
 * @file dbg.h
 * @brief Nintendo CTR-SDK Debugging Library (`nn::dbg`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Handle.h"

#ifdef __cplusplus
#include "nn/dbg/dbg_Types.h"
#include "nn/dbg/dbg_Api.h"

namespace nn {
namespace dbg {

    void Panic(const char* file, int line, const char* format, ...);
    void SetThreadName(Handle thread, const char* name);

} // namespace dbg
} // namespace nn

#else /* !__cplusplus (Pure C) */

void nn_dbg_Panic(const char* file, int line, const char* format, ...);

#endif /* __cplusplus */
