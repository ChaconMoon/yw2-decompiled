#include <ctr/dsp.h>
#include <ctr/svc.h>

/**
 * @brief func_00138590
 * @address 0x00138590
 * nn::dsp handle query and replacement sequence.
 *
 * Queries a new kernel handle into a stack variable via func_00148084(), invokes
 * func_001371D4() if the result has bit 31 set (negative error), closes any
 * previously stored handle at *pHandle via svc 0x23 (svcCloseHandle), and
 * stores the newly acquired handle into *pHandle.
 *
 * @param[in,out] pHandle Pointer to the kernel Handle slot to update.
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
