#include <ctr/dsp.h>

/**
 * @brief func_0013873C
 * @address 0x0013873C
 * nn::dsp conditional sub-component pointer getter (+0x14) with error helper call.
 *
 * Checks if g_DspState_008E5200.flag_04 is non-zero. If active, stores the address
 * of g_DspState_008E5200.sub_14 (+0x14) into *outPtr and returns 0. Otherwise,
 * stores NULL into *outPtr and returns the error result from func_001481E8(43).
 *
 * @param[out] outPtr Destination pointer to receive &g_DspState_008E5200.sub_14 or NULL.
 * @return s32 0 on success, or error code from func_001481E8(43) if inactive.
 */
s32 func_0013873C(void **outPtr)
{
        void *ptr = (g_DspState_008E5200.flag_04 == 0) ? NULL : &g_DspState_008E5200.sub_14;
        *outPtr   = ptr;
        return (ptr == NULL) ? func_001481E8(43) : 0;
}
