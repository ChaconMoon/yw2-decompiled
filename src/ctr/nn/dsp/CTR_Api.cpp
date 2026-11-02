/**
 * @file CTR_Api.cpp
 * @brief Nintendo CTR-SDK nn::dsp::CTR high-level service API & state management.
 *
 * Reference: Nintendo 3DS CTR-SDK (nn/dsp/CTR/CTR_Api.cpp)
 */

#include "nn/dsp/CTR/CTR_Api.h"
#include "ctr/svc.h"
#include "ctr/applet.h"
#include "ctr/dsp.h"

extern "C" {

#ifndef CTR_API_PASS_B1049

/**
 * @brief Sets a value in the Thread Local Storage (TLS) array.
 * @address 0x00136780
 * @size 16 bytes
 *
 * @param[in] pSlot Pointer to the TLS slot index offset.
 * @param[in] value Value to store in the designated slot.
 */
#pragma push
#pragma Otime
void func_00136780(const u32 *pSlot, u32 value)
{
    u32 *tls;
    CTR_GET_TLS(tls);
    tls[*pSlot] = value;
}
#pragma pop

/**
 * @brief Returns the 32-bit state word stored at unk_00815DFC.
 * @address 0x00137054
 * @size 16 bytes
 *
 * @return 32-bit state word.
 */
u32 func_00137054(void)
{
    return unk_00815DFC;
}

/**
 * @brief Closes a DSP handle and resets its stored pointer to invalid.
 * @address 0x00137114
 * @size 60 bytes
 *
 * @param[in,out] pHandle Pointer to the handle variable to close and reset.
 * @return Result code of the svcCloseHandle call.
 */
u32 func_00137114(u32 *pHandle)
{
    u32 res;

    if (*pHandle != 0)
    {
        u32 err;
        res = svcCloseHandle_inline(*pHandle);
        err = res >> 31;
        if (err != 0)
        {
            func_001371D4();
        }
        *pHandle = unk_00708D68;
    }
    else
    {
        res = 0xD8A103F7;
    }

    return res;
}

/**
 * @brief Conditional handle close and state flag reset on unk_00828EAD.
 * @address 0x001384AC
 * @size 56 bytes
 *
 * @param[in,out] pHandle Pointer to the handle variable.
 */
#pragma push
#pragma Otime
void func_001384AC(u32 *pHandle)
{
    if (unk_00828EAD != 0)
    {
        u32 res;
        func_00148024();
        res = svcCloseHandle_inline(*pHandle);
        res >>= 31;
        if (res != 0)
        {
            func_001371D4();
        }
        unk_00828EAD = 0;
    }
}
#pragma pop

/**
 * @brief Queries a new kernel handle and replaces *pHandle.
 * @address 0x00138590
 * @size 64 bytes
 *
 * @param[in,out] pHandle Pointer to the handle to update.
 */
void func_00138590(u32 *pHandle)
{
    u32 localHandle = 0;
    u32 res         = func_00148084(&localHandle);
    res >>= 31;
    if (res != 0)
    {
        func_001371D4();
    }
    if (*pHandle != 0)
    {
        svcCloseHandle_inline(*pHandle);
        *pHandle = 0;
    }
    *pHandle = localHandle;
}

/**
 * @brief Status check and conditional error handler call to func_001371D4.
 * @address 0x001385D0
 * @size 28 bytes
 */
#pragma push
#pragma Otime
void func_001385D0(void)
{
    u32 res = func_001480C0() >> 31;
    if (res)
    {
        func_001371D4();
    }
}
#pragma pop

/**
 * @brief Validates that a string pointer is non-NULL and points to a non-zero character.
 * @address 0x001386D4
 * @size 24 bytes
 *
 * @param[in] str Input string or byte buffer pointer.
 * @return TRUE (1) if pointer is non-NULL and *str != 0, FALSE (0) otherwise.
 */
bool32 func_001386D4(const char *str)
{
    bool32 ret = TRUE;

    if (str == NULL || *str == '\0')
    {
        ret = FALSE;
    }

    return ret;
}

/**
 * @brief Checks if a string starts with the DSP firmware prefix "FGONLYT".
 * @address 0x001386EC
 * @size 52 bytes
 *
 * @param[in] str Input string pointer.
 * @return TRUE (1) if str begins with "FGONLYT", FALSE (0) otherwise.
 */
bool32 func_001386EC(const char *str)
{
    bool32 match = 0;
    u32 len = func_002FEEBC(s_FGONLYT_007132D4);

    if (func_0013EC88(str, s_FGONLYT_007132D4, len) == 0)
    {
        match = 1;
    }

    return match;
}

/**
 * @brief Bounded string length (strnlen).
 * @address 0x001388CC
 * @size 32 bytes
 *
 * @param[in] str     Pointer to null-terminated string.
 * @param[in] max_len Maximum length to scan.
 * @return Length of string up to max_len.
 */
u32 func_001388CC(const char *str, u32 max_len)
{
    u32 i = 0;

    while (1)
    {
        if (str[i] == '\0' || max_len <= i)
        {
            break;
        }
        i++;
    }

    return i;
}

/**
 * @brief Reads the 8-bit signed value stored at unk_00815DAC.
 * @address 0x001389BC
 * @size 16 bytes
 *
 * @return Signed 8-bit state value.
 */
s8 func_001389BC(void)
{
    return unk_00815DAC;
}

#else /* CTR_API_PASS_B1049 */

/**
 * @brief Checks DSP initialization state and conditionally triggers callback dispatch.
 * @address 0x001389CC
 * @size 40 bytes
 *
 * @return u32 Result code: 0x00202BF8 if uninitialized, or return value of func_001488F0.
 */
#pragma push
#pragma Otime
u32 func_001389CC(void)
{
    u32 res;

    if (func_001389BC() == 0)
    {
        res = 0x00202BF8;
    }
    else
    {
        func_0016C994();
        res = func_001488F0();
    }

    return res;
}
#pragma pop

#endif /* CTR_API_PASS_B1049 */

#ifndef CTR_API_PASS_B1049

/**
 * @brief Reads the 8-bit signed value stored at unk_00815CA0.
 * @address 0x00138A80
 * @size 16 bytes
 *
 * @return Signed 8-bit state value.
 */
s8 func_00138A80(void)
{
    return unk_00815CA0;
}

/**
 * @brief Reads the 32-bit word at offset 0x10 from g_AppletState_00814E00.
 * @address 0x00138A90
 * @size 16 bytes
 *
 * @return 32-bit word value.
 */
u32 func_00138A90(void)
{
    return nn::applet::detail::g_AppletState_00814E00.unk_10;
}

/**
 * @brief Invokes func_001365E4 passing &unk_00815C78.
 * @address 0x00138AA0
 * @size 12 bytes
 */
void func_00138AA0(void)
{
    func_001365E4(&unk_00815C78);
}

/**
 * @brief Invokes func_0016A89C passing &unk_00815C78.
 * @address 0x00138AAC
 * @size 12 bytes
 */
void func_00138AAC(void)
{
    func_0016A89C(&unk_00815C78);
}

/**
 * @brief Invokes func_00136594 passing &unk_00815C78.
 * @address 0x00138B58
 * @size 12 bytes
 */
void func_00138B58(void)
{
    func_00136594(&unk_00815C78);
}

#endif /* !CTR_API_PASS_B1049 */

} // extern "C"
