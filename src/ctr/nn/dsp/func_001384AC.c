#include <ctr/dsp.h>
#include <ctr/svc.h>

/**
 * @brief func_001384AC
 * @address 0x001384AC
 * nn::dsp conditional handle close (svc 0x23) and state flag reset on unk_00828EAD.
 *
 * Checks if global flag unk_00828EAD is non-zero. If active, invokes func_00148024(),
 * closes the kernel handle at *pHandle via svc 0x23 (svcCloseHandle), calls
 * func_001371D4() if the result is negative (bit 31 set), and clears unk_00828EAD.
 *
 * @param[in] pHandle Pointer to the kernel Handle to close.
 */
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
