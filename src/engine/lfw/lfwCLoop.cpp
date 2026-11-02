/**
 * @file lfwCLoop.cpp
 * @brief Level-5 Framework base loop state machine (`lfwCLoop`).
 */

#include <engine/lfw.h>
#include <ctr/applet.h>

extern "C" {

#ifdef __ARMCC_VERSION
#pragma push
#pragma Otime
#endif

/**
 * @brief func_0010B888
 * @address 0x0010B888
 * lfwCLoop state and channel reset sequence.
 *
 * @param[out] state Pointer to the LfwState_0010B888 structure to reset.
 */
void func_0010B888(LfwState_0010B888 *state)
{
    state->field_00 = -1;
    state->field_04 = 0;
    state->field_44 = 0;
    state->field_48 = 0;
    state->field_4C = 0;
    state->field_50 = 0;
    state->field_54 = 0;
    state->field_58 = 0;
    state->field_5C = 0;
}

#ifdef __ARMCC_VERSION
#pragma pop
#endif

/**
 * @brief func_0010BBB8
 * @address 0x0010BBB8
 * Level-5 Framework global pointer setter (0x00815DA8)
 *
 * @param[in] ptr Pointer/value to store into the global slot
 */
void func_0010BBB8(void *ptr)
{
    unk_00815DA8 = ptr;
}

/**
 * @brief func_0010C598
 * @address 0x0010C598
 * Level-5 Framework applet attribute getter
 * Reads the 32-bit attribute field at offset 0x18 from the global applet state (0x00814E00).
 *
 * @return u32 The applet attribute bitfield
 */
u32 func_0010C598(void)
{
    return g_AppletState_00814E00.attribute;
}

/**
 * @brief func_0010C5C8
 * @address 0x0010C5C8
 * Level-5 Framework applet attribute setter.
 * Stores the given 32-bit attribute value into g_AppletState_00814E00.attribute (offset 0x18).
 *
 * @param[in] attribute The new applet attribute bitfield value
 */
void func_0010C5C8(u32 attribute)
{
    g_AppletState_00814E00.attribute = attribute;
}

/**
 * @brief func_0010C6A4
 * @address 0x0010C6A4
 * Level-5 Framework applet unk_14 setter.
 * Stores the given 32-bit value into g_AppletState_00814E00.unk_14 (offset 0x14).
 *
 * @param[in] val The new 32-bit value to store
 */
void func_0010C6A4(u32 val)
{
    g_AppletState_00814E00.unk_14 = val;
}

} /* extern "C" */
