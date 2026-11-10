/**
 * @file hid.h
 * @brief Nintendo CTR-SDK Human Interface Device Library (`nn::hid`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"

#ifdef __cplusplus
#include "nn/hid/CTR/hid_HidBase.h"

namespace nn {
namespace hid {

    enum Button {
        BUTTON_A      = (1 << 0),
        BUTTON_B      = (1 << 1),
        BUTTON_SELECT = (1 << 2),
        BUTTON_START  = (1 << 3),
        BUTTON_RIGHT  = (1 << 4),
        BUTTON_LEFT   = (1 << 5),
        BUTTON_UP     = (1 << 6),
        BUTTON_DOWN   = (1 << 7),
        BUTTON_R      = (1 << 8),
        BUTTON_L      = (1 << 9),
        BUTTON_X      = (1 << 10),
        BUTTON_Y      = (1 << 11),
        BUTTON_DEBUG  = (1 << 12),
        BUTTON_ZL     = (1 << 14),
        BUTTON_ZR     = (1 << 15),
        BUTTON_TOUCH  = (1 << 20),
        BUTTON_CSTICK_RIGHT = (1 << 24),
        BUTTON_CSTICK_LEFT  = (1 << 25),
        BUTTON_CSTICK_UP    = (1 << 26),
        BUTTON_CSTICK_DOWN  = (1 << 27)
    };

    struct PadStatus {
        u32 hold;
        u32 trigger;
        u32 release;
        s16 stickX;
        s16 stickY;
    };

    struct TouchPanelStatus {
        u16 x;
        u16 y;
        u8  touch;
    };

    struct AccelerometerStatus {
        s16 x;
        s16 y;
        s16 z;
    };

    struct GyroscopeStatus {
        s16 x;
        s16 y;
        s16 z;
    };

    /* HID Reader Management */
    Result Initialize();
    Result Finalize();

    void   GetPadStatus(PadStatus* out);
    void   GetTouchPanelStatus(TouchPanelStatus* out);
    void   GetAccelerometerStatus(AccelerometerStatus* out);
    void   GetGyroscopeStatus(GyroscopeStatus* out);

    namespace CTR {
        using ::nn::hid::CTR::HidBase;
    } // namespace CTR

} // namespace hid
} // namespace nn

#else /* !__cplusplus (Pure C) */

typedef struct nn_hid_PadStatus {
    u32 hold;
    u32 trigger;
    u32 release;
    s16 stickX;
    s16 stickY;
} nn_hid_PadStatus;

typedef struct nn_hid_TouchPanelStatus {
    u16 x;
    u16 y;
    u8  touch;
} nn_hid_TouchPanelStatus;

s32  nn_hid_Initialize(void);
void nn_hid_Finalize(void);
void nn_hid_GetPadStatus(nn_hid_PadStatus* out);
void nn_hid_GetTouchPanelStatus(nn_hid_TouchPanelStatus* out);

#endif /* __cplusplus */
