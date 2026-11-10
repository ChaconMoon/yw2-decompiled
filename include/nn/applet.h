/**
 * @file applet.h
 * @brief Nintendo CTR-SDK Applet Controller Library (`nn::applet`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Handle.h"
#include "nn/Result.h"
#include "ctr/applet.h"

#ifdef __cplusplus

namespace nn {
namespace applet {

    enum AppletMode {
        APPLET_MODE_APPLICATION = 0,
        APPLET_MODE_SYSTEM      = 1,
        APPLET_MODE_LIBRARY     = 2
    };

    enum AppletType {
        APPLET_TYPE_NONE    = 0,
        APPLET_TYPE_SYS     = 1,
        APPLET_TYPE_LIB     = 2,
        APPLET_TYPE_APP     = 3
    };

    /* Applet Lifecycle Control */
    void Initialize();
    void Finalize();
    bool IsActive();
    void Enable(bool enable);
    void CloseApplication();

    namespace CTR {
        /* Already declared in ctr/applet.h: bool IsApplication(); (0x001284E4) */
        bool IsApplet();
    } // namespace CTR

} // namespace applet
} // namespace nn

#else /* !__cplusplus (Pure C) */

bool32 nn_applet_IsActive(void);
void   nn_applet_CloseApplication(void);

#endif /* __cplusplus */
