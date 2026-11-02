#include "ctr/dsp.h"
#include "ctr/svc.h"

/**
 * @brief Closes a DSP handle and resets its stored pointer to invalid.
 *
 * If the handle is non-zero, closes it via svcCloseHandle (svc 0x23), checks for
 * error status via func_001371D4, and resets the handle value to unk_00708D68 (0).
 * If the handle is already null/zero, returns error code 0xD8A103F7.
 *
 * Address: 0x00137114
 * Size:    68 bytes (60 B code + 8 B literal pool)
 *
 * @param pHandle Pointer to the handle to be closed and reset.
 * @return u32 nn::Result status code from svcCloseHandle or 0xD8A103F7 on error.
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
