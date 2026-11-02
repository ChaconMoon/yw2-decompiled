/**
 * @file lfwCLoopEmpty.cpp
 * @brief Level-5 Framework empty loop state (`lfwCLoopEmpty`).
 */

#include <engine/lfw.h>

extern "C" {

extern const char s_lfwCLoopEmpty[];

/**
 * @brief func_0010B6D0
 * @address 0x0010B6D0
 * lfwCLoopEmpty virtual method (returns constant 0, table 0x00107488)
 */
s32 func_0010B6D0(void)
{
    return 0;
}

/**
 * @brief func_0010B6D8
 * @address 0x0010B6D8
 * lfwCLoopEmpty::GetClassName getter via literal pool (0x008602F8)
 */
const char *func_0010B6D8(void)
{
    return s_lfwCLoopEmpty;
}

} /* extern "C" */
