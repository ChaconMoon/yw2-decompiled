#include <ctr/dsp.h>

/**
 * @brief func_001387B0
 * @address 0x001387B0
 * nn::dsp conditional sub-component pointer getter (+0x18) with error helper call.
 *
 * Checks if g_DspState_008E5200.flag_05 is non-zero. If active, stores the address
 * of g_DspState_008E5200.sub_18 (+0x18) into *outPtr and returns 0. Otherwise,
 * stores NULL into *outPtr and returns the error result from func_001481E8(43).
 *
 * @param[out] outPtr Destination pointer to receive &g_DspState_008E5200.sub_18 or NULL.
 * @return s32 0 on success, or error code from func_001481E8(43) if inactive.
 */
s32 func_001387B0(void **outPtr)
{
        void *ptr = (g_DspState_008E5200.flag_05 == 0) ? NULL : &g_DspState_008E5200.sub_18;
        *outPtr   = ptr;
        return (ptr == NULL) ? func_001481E8(43) : 0;
}
