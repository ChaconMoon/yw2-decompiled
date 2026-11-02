/**
 * @file snd_Dspsnd.cpp
 * @brief Nintendo CTR-SDK nn::snd sound driver interface for DSP audio channels.
 *
 * Implements channel descriptors, voice playback, audio buffer calculation,
 * and interface cleanup for hardware audio mixing via Teak DSP.
 * Reference: ACNL decomp (lib/nn/src/nn/snd/CTR/snd_Dspsnd.cpp)
 */

#include "nn/snd/CTR/snd_Dspsnd.h"
#include "ctr/svc.h"
#include "ctr/dsp.h"

extern "C" {

#if !defined(SND_DSPSND_PASS_B1049) && !defined(SND_DSPSND_PASS_B561)

/**
 * @brief Jump thunk (trampoline) forwarding to func_0013457C.
 * @address 0x0013628C
 * @size 4 bytes
 *
 * @param[in] arg0 Context pointer passed directly to func_0013457C.
 */
void thunk_FUN_0013457c(void *arg0)
{
    func_0013457C(arg0);
}

/**
 * @brief Initializes a DSP channel header.
 * @address 0x00136318
 * @size 20 bytes
 *
 * Sets the magic identifier (0xE7E3FFFF) and clears the status field to 0.
 *
 * @param[out] header Pointer to the DSP header structure to initialize.
 */
void func_00136318(DspHeader_00136318 *header)
{
    header->magic = 0xE7E3FFFF;
    header->status = 0;
}

/**
 * @brief Destroys and cleans up a DSP object's child interface.
 * @address 0x00136330
 * @size 60 bytes
 *
 * @param[in,out] self Pointer to the DspObject_00136318 instance.
 * @return DspObject_00136318* Pointer to the same instance (self).
 */
DspObject_00136318* func_00136330(DspObject_00136318 *self)
{
    uintptr_t *pChild = &self->child;
    uintptr_t  val = *pChild;

    if ((val & ~1) != 0)
    {
        if ((val & 1) != 0)
        {
            func_001371D4();
        }

        func_00136420((DspInterface_00136420 *)(*pChild & ~1));
        *pChild = 0;
    }

    return self;
}

/**
 * @brief Sets up stack structure and calls func_00146570.
 * @address 0x0013636C
 * @size 32 bytes
 *
 * @param[in] arg0 Context pointer passed through to func_00146570.
 */
#pragma push
#pragma O2
#pragma Otime
void func_0013636C(void *arg0)
{
    u32 temp = func_0012746C();
    func_00146570(&temp, arg0);
}
#pragma pop

/**
 * @brief Virtual method dispatch at vtable index 12 (tail-call).
 * @address 0x00136420
 * @size 24 bytes
 *
 * @param[in] obj Pointer to DspInterface_00136420 object.
 */
#pragma push
#pragma O1
void func_00136420(DspInterface_00136420 *obj)
{
    if (obj == NULL)
    {
        return;
    }
    obj->vtable[12](obj);
}
#pragma pop

/**
 * @brief Clears channel bit in global bitmask unk_00828E90.
 * @address 0x00136804
 * @size 52 bytes
 *
 * @param[in,out] obj Pointer to DspChannel_00136804.
 * @return Pointer to DspChannel_00136804.
 */
#pragma push
#pragma O2
#pragma Otime
DspChannel_00136804 *func_00136804(DspChannel_00136804 *obj)
{
    u32 index = obj->index;

    if (index < 16)
    {
        u16 *p = &unk_00828E90;
        if ((*p >> index) & 1)
        {
            *p &= ~(1 << index);
        }
    }
    return obj;
}
#pragma pop

/**
 * @brief Subcomponent cleanup and reference counter decrement.
 * @address 0x001375DC
 * @size 32 bytes
 *
 * @param[in,out] obj Pointer to DspComponent_001375DC.
 */
void func_001375DC(DspComponent_001375DC *obj)
{
    func_0013769C(&obj->sub_18);
    obj->count_54 -= 1;
}

/**
 * @brief Exchanges channel sub-descriptor 16-bit field value.
 * @address 0x00137920
 * @size 16 bytes
 *
 * @param[in,out] channel Pointer to DspChannel_00137920.
 * @param[in]     val     New 16-bit field value.
 * @return Previous 16-bit field value.
 */
u16 func_00137920(DspChannel_00137920 *channel, u16 val)
{
    DspSubChannel *sub = &channel->sub;
    u16 old = sub->field_10;
    sub->field_10 = val;
    return old;
}

/**
 * @brief Exchanges bit 0 flag in flags_12 field.
 * @address 0x00137930
 * @size 32 bytes
 *
 * @param[in,out] channel Pointer to DspChannel_00137920.
 * @param[in]     flag    New flag bit value.
 * @return Previous bit 0 value.
 */
u16 func_00137930(DspChannel_00137920 *channel, u16 flag)
{
    DspSubChannel *sub = &channel->sub;
    u16 flags = sub->flags_12;
    u16 old = flags & 1;
    sub->flags_12 = (flags & ~1) | (flag & 1);
    return old;
}

/**
 * @brief Exchanges channel sub-descriptor 8-bit signed state.
 * @address 0x00137A0C
 * @size 16 bytes
 *
 * @param[in,out] channel Pointer to DspChannel_00137920.
 * @param[in]     state   New signed 8-bit state.
 * @return Previous signed 8-bit state.
 */
s8 func_00137A0C(DspChannel_00137920 *channel, s8 state)
{
    DspSubChannel *sub = &channel->sub;
    s8 old = sub->field_14;
    sub->field_14 = state;
    return old;
}

/**
 * @brief Sound voice wrapper calling func_00147C74 with &unk_0088A85C.
 * @address 0x00137F18
 * @size 16 bytes
 *
 * @param[in] param Pointer passed to func_00147C74.
 */
void func_00137F18(void *param)
{
    func_00147C74(&unk_0088A85C, param);
}

/**
 * @brief Conditional pair setter for channel audio parameters (+0x14, +0x18).
 * @address 0x00137F28
 * @size 24 bytes
 *
 * @param[in,out] obj    Pointer to DspPair_00137F28 structure.
 * @param[in]     param1 First parameter pointer.
 * @param[in]     param2 Second parameter pointer.
 * @return TRUE if param1 was non-NULL, FALSE otherwise.
 */
bool32 func_00137F28(DspPair_00137F28 *obj, void *param1, void *param2)
{
    if (param1 == NULL)
    {
        return FALSE;
    }

    obj->field_14 = param1;
    obj->field_18 = param2;
    return TRUE;
}

/**
 * @brief Computes buffer size in bytes for PCM audio samples.
 * @address 0x00137F40
 * @size 28 bytes
 *
 * @param[in] obj Pointer to DspBuffer_00137F40 structure.
 * @return Buffer size in bytes.
 */
u32 func_00137F40(DspBuffer_00137F40 *obj)
{
    u32 arg0 = obj->field_3C;
    u8 arg1 = obj->field_55;
    arg0 *= arg1;
    arg0 *= 5;
    arg0 <<= 7;
    return arg0 + 32;
}

/**
 * @brief Conditional pair setter for channel audio parameters (+0x28, +0x2C).
 * @address 0x0013810C
 * @size 24 bytes
 *
 * @param[in,out] obj    Pointer to DspPair_00137F28 structure.
 * @param[in]     param1 First parameter pointer.
 * @param[in]     param2 Second parameter pointer.
 * @return TRUE if param1 was non-NULL, FALSE otherwise.
 */
bool32 func_0013810C(DspPair_00137F28 *obj, void *param1, void *param2)
{
    if (param1 == NULL)
    {
        return FALSE;
    }

    obj->field_28 = param1;
    obj->field_2C = param2;
    return TRUE;
}

#endif /* !SND_DSPSND_PASS_B1049 && !SND_DSPSND_PASS_B561 */

#ifdef SND_DSPSND_PASS_B1049

/**
 * @brief func_0013873C
 * @address 0x0013873C
 * nn::snd conditional sub-component pointer getter (+0x14) with error helper call.
 */
s32 func_0013873C(void **outPtr)
{
    void *ptr = (g_DspState_008E5200.flag_04 == 0) ? NULL : &g_DspState_008E5200.sub_14;
    *outPtr   = ptr;
    return (ptr == NULL) ? func_001481E8(43) : 0;
}

/**
 * @brief func_001387B0
 * @address 0x001387B0
 * nn::snd conditional sub-component pointer getter (+0x18) with error helper call.
 */
s32 func_001387B0(void **outPtr)
{
    void *ptr = (g_DspState_008E5200.flag_05 == 0) ? NULL : &g_DspState_008E5200.sub_18;
    *outPtr   = ptr;
    return (ptr == NULL) ? func_001481E8(43) : 0;
}

#endif /* SND_DSPSND_PASS_B1049 */

#ifdef SND_DSPSND_PASS_B561

/**
 * @brief func_00138770
 * @address 0x00138770
 * nn::snd sequential sub-buffer registration (IDs 53 and 54) and global status return.
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

#endif /* SND_DSPSND_PASS_B561 */

} // extern "C"
