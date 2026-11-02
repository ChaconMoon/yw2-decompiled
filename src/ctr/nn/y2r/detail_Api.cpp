/**
 * @file detail_Api.cpp
 * @brief Nintendo 3DS Y2R internal IPC and driver details (`nn::y2r::CTR::detail`).
 */

#include <nn/y2r/CTR/detail/detail_Api.h>
#include <ctr/dsp.h>

extern "C" {

extern void *unk_00814F28[2];
extern const char unk_0086A420[];
extern u32 func_0011F51C(void *arg0);
extern void func_00125BF0(void *arg0, const char *file, u32 line, u32 errCode);
extern void func_0011F46C(void);
extern void func_0011FA98(void);
extern void func_0011FA7C(void);
extern void func_0011FDAC(Y2rContext *ctx);

/**
 * @brief func_00113700
 * @address 0x00113700
 * Y2R buffer deallocation sequence on unk_00814F28
 *
 * @return Returns 1 on successful deallocation of both buffers
 */
s32 func_00113700(void)
{
    if (unk_00814F28[1] != 0)
    {
        delete (char *)unk_00814F28[1];
        unk_00814F28[1] = 0;
    }
    if (unk_00814F28[0] != 0)
    {
        delete (char *)unk_00814F28[0];
        unk_00814F28[0] = 0;
    }
    return 1;
}

/**
 * @brief func_001137C0
 * @address 0x001137C0
 * Helper invoking func_00136154(&unk_0086A0A4)
 */
s32 func_001137C0(void)
{
    func_00136154(&unk_0086A0A4);
    return 1;
}

/**
 * @brief func_001138D0
 * @address 0x001138D0
 * Y2R error reporting assertion thunk invoking func_0011F51C and tail calling func_00125BF0
 * (Source assertion at line 1078 of unk_0086A420)
 *
 * @param arg0 Device or driver context instance pointer
 */
void func_001138D0(void *arg0)
{
    u32 err = func_0011F51C(arg0);
    func_00125BF0(arg0, unk_0086A420, 1078, err);
}

/**
 * @brief func_00113F00
 * @address 0x00113F00
 * Y2R initialization sequence invoking func_0011F46C, func_0011FA98, func_0011FA7C
 *
 * @return Returns 1 on successful initialization
 */
s32 func_00113F00(void)
{
    func_0011F46C();
    func_0011FA98();
    func_0011FA7C();
    return 1;
}

/**
 * @brief func_00113F4C
 * @address 0x00113F4C
 * Y2R tail-call thunk to func_0011FDAC with global unk_008661EC
 */
void func_00113F4C(void)
{
    func_0011FDAC(&unk_008661EC);
}

/**
 * @brief func_00113F58
 * @address 0x00113F58
 * Y2R global bit 3 flag getter via literal pool (0x008661EC)
 */
u32 func_00113F58(void)
{
    return (unk_008661EC.flag >> 3) & 1;
}

/**
 * @brief func_00113F70
 * @address 0x00113F70
 * Y2R global float field getter via literal pool (0x00815090)
 */
f32 func_00113F70(void)
{
    return unk_00815090.value;
}

} /* extern "C" */
