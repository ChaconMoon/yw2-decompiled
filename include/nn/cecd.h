/**
 * @file cecd.h
 * @brief Nintendo CTR-SDK StreetPass Library (`nn::cecd`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus

namespace nn {
namespace cecd {

    Result Initialize();
    Result Finalize();
    Result ReadMessage(u32 boxId, void* buffer, u32 size, u32* pOutReadSize);
    Result WriteMessage(u32 boxId, const void* buffer, u32 size);
    Result GetMessageCount(u32 boxId, u32* pOutCount);

} // namespace cecd
} // namespace nn

#else /* !__cplusplus (Pure C) */

s32  nn_cecd_Initialize(void);
void nn_cecd_Finalize(void);

#endif /* __cplusplus */
