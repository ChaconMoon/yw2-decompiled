/**
 * @file y2r_Y2r.cpp
 * @brief Nintendo 3DS YUV-to-RGB Conversion (`nn::y2r::CTR`) implementation.
 */

#include <nn/y2r/CTR/y2r_Y2r.h>
#include <game/core.h>

extern "C" {

extern void *unk_00867CF8;
extern void func_0012F804(void *mutex);
extern void func_00155E8C(void *mutex);
extern void func_002FDAC4(u32 arg0, void *outPtr);

/**
 * @brief func_00112AA8
 * @address 0x00112AA8
 * Y2R channel state structure field zero-initialization
 *
 * @param dev Pointer to the Y2R device context holding channel state
 */
void func_00112AA8(Y2rDevice *dev)
{
    dev->state.field_524 = 0;
    dev->state.field_528 = 0;
    dev->state.field_530 = 0;
    dev->state.field_534 = 0;
    dev->state.field_538 = 0;
    dev->state.field_53C = 0;
    dev->state.field_540 = 0;
    dev->state.field_544 = 0;
    dev->state.field_548 = 0;
    dev->state.field_54C = 0;
    dev->state.field_550 = 0;
    dev->state.field_554 = 0;
    dev->state.field_558 = 0;
    dev->state.field_55C = 0;
    dev->state.field_560 = 0;
    dev->state.field_568 = 0;
    dev->state.field_56C = 0;
    dev->state.field_564 = 0;
    dev->state.flag_570 = 0;
    dev->state.flag_571 = 0;
    dev->state.flag_572 = 0;
    dev->state.flag_573 = 0;
}

/**
 * @brief func_00112D8C
 * @address 0x00112D8C
 * Method chaining wrapper invoking func_0014A06C
 */
void *func_00112D8C(void *self)
{
    func_0014A06C(self);
    return self;
}

/**
 * @brief func_00112E60
 * @address 0x00112E60
 * Y2R struct offset 8648 u16 reader
 */
u16 func_00112E60(const Y2rObject *y2rObject)
{
    const Y2rSubContext *sub = &y2rObject->sub;
    __nop();
    return sub->flag;
}

/**
 * @brief func_00112EC8
 * @address 0x00112EC8
 * Y2R mutex lock/unlock sequence on unk_00867CF8
 */
void func_00112EC8(void)
{
    func_0012F804(&unk_00867CF8);
    func_00155E8C(&unk_00867CF8);
}

/**
 * @brief func_00112EE4
 * @address 0x00112EE4
 * Y2R stack buffer query invoking func_002FDAC4
 *
 * @param arg0 Configuration or query parameter passed in r0
 */
void func_00112EE4(u32 arg0)
{
    u32 stackVar;
    func_002FDAC4(arg0, &stackVar);
}

} /* extern "C" */
