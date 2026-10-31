#include <ctr/dsp.h>

/**
 * @brief func_00138770
 * @address 0x00138770
 * nn::dsp sequential sub-buffer registration (IDs 53 and 54) and global status return.
 *
 * Resets global status unk_00829A74 to 0, invokes func_00148798 for sub-buffer +0x00
 * (ID 53, size/count 2) and sub-buffer +0x08 (ID 54, size/count 1024), and returns
 * the resulting status stored in unk_00829A74.
 *
 * @param[in,out] ctx Pointer to DspContext_00138770 structure.
 * @return s32 Resulting status code from unk_00829A74.
 */
s32 func_00138770(DspContext_00138770 *ctx)
{
        DspContext_00138770 *p  = ctx;
        void                *p0 = p->sub_00;
        unk_00829A74            = 0;
        func_00148798(53, p0, 2);
        func_00148798(54, p->sub_08, 1024);
        return unk_00829A74;
}
