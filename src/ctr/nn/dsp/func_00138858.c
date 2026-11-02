#include "ctr/dsp.h"
#include "ctr/svc.h"

/**
 * @brief Sends a synchronous IPC request to the CTR DSP sysmodule (command 0x000E0000).
 *
 * In Nintendo 3DS Horizon OS (microkernel architecture), user processes communicate
 * with system services via Inter-Process Communication (IPC). Each thread has a
 * Thread-Local Storage (TLS) block located at the address returned by CP15 register
 * c13, c0, 3 (TPIDRURO).
 *
 * The IPC Command Buffer (cmdbuf) is mapped within the TLS starting at offset +0x80
 * (+128 bytes). This function constructs command header 0x000E0000 (Command ID 0x000E,
 * with 0 normal parameter words and 0 translate descriptors), invokes the kernel
 * via svc 0x32 (svcSendSyncRequest), and extracts the response from cmdbuf[1] on success.
 *
 * Address: 0x00138858
 * Size:    40 bytes
 *
 * @param pHandle Pointer to the service session handle.
 * @return Result code from svcSendSyncRequest, or response value from cmdbuf[1] on success.
 */
s32 func_00138858(const u32 *pHandle)
{
        u32 *cmdbuf;
        s32 res;

        /* Retrieve TLS base pointer from ARM11 CP15 coprocessor */
        CTR_GET_TLS(cmdbuf);

        /* Access IPC Command Buffer located at TLS offset +0x80 (128 bytes) */
        cmdbuf = (u32 *)((char *)cmdbuf + 128);

        /* Write IPC command header 0x000E0000 (Command ID: 0x000E, 0 normal words, 0 descriptors) */
        *cmdbuf = 0x000E0000;

        /* Issue synchronous IPC request to the DSP sysmodule via session handle */
        res = svcSendSyncRequest_inline(*pHandle);

        /* Check if Result succeeded (bit 31 is 0 for success in CTR-SDK Result codes) */
        if ((s32)(res & 0x80000000) >= 0) {
                /* On success, retrieve response data placed by the DSP service in cmdbuf[1] */
                res = cmdbuf[1];
        }

        return res;
}
