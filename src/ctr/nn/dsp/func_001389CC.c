#include "ctr/dsp.h"

/**
 * @brief Checks DSP initialization state and conditionally triggers callback dispatch.
 *
 * Calls func_001389BC to inspect the global DSP state flag (unk_00815DAC). If uninitialized (0),
 * returns error code 0x00202BF8. Otherwise, invokes func_0016C994 and tail-calls func_001488F0,
 * returning its result.
 *
 * Address: 0x001389CC
 * Size:    40 bytes (36 B code + 4 B literal pool)
 *
 * @return u32 Result code: 0x00202BF8 if uninitialized, or return value of func_001488F0.
 */
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
