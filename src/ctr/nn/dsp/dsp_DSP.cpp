/**
 * @file dsp_DSP.cpp
 * @brief Nintendo CTR-SDK dsp::DSP low-level IPC communication driver.
 *
 * Implements low-level IPC requests to the Nintendo 3DS dsp::DSP sysmodule.
 * Reference: ACNL decomp (lib/nn/src/nn/dsp/CTR/dsp_DSP.cpp)
 */

#include "nn/dsp/CTR/dsp_DSP.h"
#include "ctr/svc.h"
#include "ctr/dsp.h"

extern "C" {

/**
 * @brief Sends a synchronous IPC request to write multi-byte registers to SPI2 (Instance 1).
 * @address 0x001385EC
 * @size 60 bytes
 *
 * @param[in] pHandle Pointer to the active DSP service session handle.
 * @param[in] buffer  Pointer to input data buffer to write to SPI2.
 * @param[in] size    Number of bytes to write.
 * @return nn::Result result code (0 on success).
 */
s32 func_001385EC(const u32 *pHandle, const void *buffer, u32 size)
{
    u32 *cmdbuf;
    s32 res;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    cmdbuf[0] = 0x001E0042;

    __schedule_barrier();

    cmdbuf[1] = size;
    cmdbuf[2] = (10 | size << 4);
    cmdbuf[3] = (u32)buffer;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        res = cmdbuf[1];
    }

    return res;
}

#pragma push
#pragma Otime
/**
 * @brief Sends a synchronous IPC request to the DSP service to mask and query the semaphore state.
 * @address 0x0013862C
 * @size 52 bytes
 *
 * @param[in]  pHandle       Pointer to the active DSP service session handle.
 * @param[out] pOutSemaphore Output pointer where the retrieved semaphore byte is written.
 * @return nn::Result result code (0 on success).
 */
s32 func_0013862C(const u32 *pHandle, u8 *pOutSemaphore)
{
    u32 *cmdbuf;
    s32 res;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    *cmdbuf = 0x000A0000;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        *pOutSemaphore = ((u8 *)cmdbuf)[8];
        res = cmdbuf[1];
    }

    return res;
}
#pragma pop

/**
 * @brief Sends a synchronous IPC request to query interrupt status (command 0x000E0000).
 * @address 0x00138660
 * @size 40 bytes
 *
 * @param[in] pHandle Pointer to the active DSP service session handle.
 * @return nn::Result result code (0 on success).
 */
s32 func_00138660(const u32 *pHandle)
{
    u32 *cmdbuf;
    s32 res;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    *cmdbuf = 0x000E0000;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        res = cmdbuf[1];
    }

    return res;
}

/**
 * @brief Sends a synchronous IPC request to convert process address from DSP DRAM (Instance 1).
 * @address 0x00138688
 * @size 72 bytes
 *
 * @param[in] pHandle Pointer to the active DSP service session handle.
 * @param[in] buffer  Pointer to memory buffer to be translated.
 * @param[in] size    Size of the memory buffer.
 * @param[in] flag    Translation control flag byte.
 * @return nn::Result result code (0 on success).
 */
s32 func_00138688(const u32 *pHandle, const void *buffer, u32 size, u8 flag)
{
    u32 *cmdbuf;
    s32 res;
    const u8 *pFlag = &flag;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    cmdbuf[0] = 0x000C0082;

    __schedule_barrier();

    cmdbuf[1] = size;
    ((u8 *)cmdbuf)[8] = *pFlag;

    __schedule_barrier();

    cmdbuf[3] = (10 | size << 4);
    cmdbuf[4] = (u32)buffer;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        res = cmdbuf[1];
    }

    return res;
}

/**
 * @brief Sends a synchronous IPC request to write multi-byte registers to SPI2 (Instance 2).
 * @address 0x001387E4
 * @size 60 bytes
 *
 * @param[in] pHandle Pointer to the active DSP service session handle.
 * @param[in] buffer  Pointer to input data buffer to write to SPI2.
 * @param[in] size    Number of bytes to write.
 * @return nn::Result result code (0 on success).
 */
s32 func_001387E4(const u32 *pHandle, const void *buffer, u32 size)
{
    u32 *cmdbuf;
    s32 res;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    cmdbuf[0] = 0x001E0042;

    __schedule_barrier();

    cmdbuf[1] = size;
    cmdbuf[2] = (10 | size << 4);
    cmdbuf[3] = (u32)buffer;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        res = cmdbuf[1];
    }

    return res;
}

#pragma push
#pragma Otime
/**
 * @brief Sends a synchronous IPC request to mask and query semaphore state (Instance 2).
 * @address 0x00138824
 * @size 52 bytes
 *
 * @param[in]  pHandle       Pointer to the active DSP service session handle.
 * @param[out] pOutSemaphore Output pointer where the retrieved semaphore byte is written.
 * @return nn::Result result code (0 on success).
 */
s32 func_00138824(const u32 *pHandle, u8 *pOutSemaphore)
{
    u32 *cmdbuf;
    s32 res;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    *cmdbuf = 0x000A0000;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        *pOutSemaphore = ((u8 *)cmdbuf)[8];
        res = cmdbuf[1];
    }

    return res;
}
#pragma pop

/**
 * @brief Sends a synchronous IPC request to query interrupt status (Instance 2).
 * @address 0x00138858
 * @size 40 bytes
 *
 * @param[in] pHandle Pointer to the active DSP service session handle.
 * @return nn::Result result code (0 on success).
 */
s32 func_00138858(const u32 *pHandle)
{
    u32 *cmdbuf;
    s32 res;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    *cmdbuf = 0x000E0000;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        res = cmdbuf[1];
    }

    return res;
}

/**
 * @brief Sends a synchronous IPC request to convert process address from DSP DRAM (Instance 2).
 * @address 0x00138880
 * @size 72 bytes
 *
 * @param[in] pHandle Pointer to the active DSP service session handle.
 * @param[in] buffer  Pointer to memory buffer to be translated.
 * @param[in] size    Size of the memory buffer.
 * @param[in] flag    Translation control flag byte.
 * @return nn::Result result code (0 on success).
 */
s32 func_00138880(const u32 *pHandle, const void *buffer, u32 size, u8 flag)
{
    u32 *cmdbuf;
    s32 res;
    const u8 *pFlag = &flag;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    cmdbuf[0] = 0x000C0082;

    __schedule_barrier();

    cmdbuf[1] = size;
    ((u8 *)cmdbuf)[8] = *pFlag;

    __schedule_barrier();

    cmdbuf[3] = (10 | size << 4);
    cmdbuf[4] = (u32)buffer;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        res = cmdbuf[1];
    }

    return res;
}

/**
 * @brief Sends a synchronous IPC request to the DSP service (command 0x00170000 - SetSemaphoreMask).
 * @address 0x00138A30
 * @size 40 bytes
 *
 * @param[in] pHandle Pointer to the active DSP service session handle.
 * @return nn::Result result code (0 on success).
 */
s32 func_00138A30(const u32 *pHandle)
{
    u32 *cmdbuf;
    s32 res;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    *cmdbuf = 0x00170000;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        res = cmdbuf[1];
    }

    return res;
}

/**
 * @brief Sends a synchronous IPC request to the DSP service (command 0x001A0000 - SetIirFilterI2S1).
 * @address 0x00138A58
 * @size 40 bytes
 *
 * @param[in] pHandle Pointer to the active DSP service session handle.
 * @return nn::Result result code (0 on success).
 */
s32 func_00138A58(const u32 *pHandle)
{
    u32 *cmdbuf;
    s32 res;

    CTR_GET_TLS(cmdbuf);
    cmdbuf = (u32 *)((char *)cmdbuf + 128);

    *cmdbuf = 0x001A0000;

    res = svcSendSyncRequest_inline(*pHandle);

    if ((s32)(res & 0x80000000) >= 0)
    {
        res = cmdbuf[1];
    }

    return res;
}

} // extern "C"
