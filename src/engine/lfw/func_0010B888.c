#include <engine/lfw.h>

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
