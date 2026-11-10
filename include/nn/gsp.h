/**
 * @file gsp.h
 * @brief Nintendo CTR-SDK GPU Synchronization Library (`nn::gsp`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Handle.h"
#include "nn/Result.h"

#ifdef __cplusplus
#include "nn/os/os_EventBase.h"

namespace nn {
namespace gsp {

    enum LcdId {
        LCD_TOP    = 0,
        LCD_BOTTOM = 1
    };

    enum FramebufferFormat {
        FORMAT_RGBA8  = 0,
        FORMAT_RGB8   = 1,
        FORMAT_RGB565 = 2,
        FORMAT_RGB5_A1= 3,
        FORMAT_RGBA4  = 4
    };

    enum InterruptId {
        INTERRUPT_PSC0 = 0,
        INTERRUPT_PSC1 = 1,
        INTERRUPT_VBLANK0 = 2, /* Top screen VBlank */
        INTERRUPT_VBLANK1 = 3, /* Bottom screen VBlank */
        INTERRUPT_PPF = 4,
        INTERRUPT_P3D = 5,
        INTERRUPT_DMA = 6
    };

    struct FramebufferInfo {
        uintptr_t topColorBuffer0;
        uintptr_t topColorBuffer1;
        uintptr_t bottomColorBuffer0;
        uintptr_t bottomColorBuffer1;
        u32       format;
    };

    Result Initialize();
    Result Finalize();

    Result RegisterInterruptEvent(InterruptId id, Handle event);
    Result UnregisterInterruptEvent(InterruptId id);
    Result SetLcdForceBlack(LcdId lcd, bool forceBlack);
    Result TriggerCmdReqQueue();

    namespace CTR {
        void SetLedPattern(u32 pattern);
    } // namespace CTR

} // namespace gsp
} // namespace nn

#else /* !__cplusplus (Pure C) */

typedef struct nn_gsp_FramebufferInfo {
    uintptr_t topColorBuffer0;
    uintptr_t topColorBuffer1;
    uintptr_t bottomColorBuffer0;
    uintptr_t bottomColorBuffer1;
    u32       format;
} nn_gsp_FramebufferInfo;

s32  nn_gsp_Initialize(void);
void nn_gsp_Finalize(void);
s32  nn_gsp_SetLcdForceBlack(u32 lcd, bool32 forceBlack);

#endif /* __cplusplus */
