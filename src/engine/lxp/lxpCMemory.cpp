/**
 * @file lxpCMemory.cpp
 * @brief Level-5 Cross-Platform Memory Manager (`lxpCMemory`).
 */

#include <engine/lxp.h>

extern "C" {

/**
 * @brief func_0012CB00
 * @address 0x0012CB00
 * Level-5 engine global singleton getter via literal pool (0x00868A9C)
 */
void *func_0012CB00(void)
{
    return &unk_00868A9C;
}

/**
 * @brief func_0012FEC0
 * @address 0x0012FEC0
 * lxpCMemory boolean true stub (returns constant 1)
 */
s32 func_0012FEC0(void)
{
    return 1;
}

/**
 * @brief func_00130018
 * @address 0x00130018
 * lxpCMemory virtual boolean true stub (returns constant 1)
 */
s32 func_00130018(void)
{
    return 1;
}

/**
 * @brief func_001300B0
 * @address 0x001300B0
 * lxpCMemory manager pointer getter via literal pool (0x00868F10)
 */
void *func_001300B0(void)
{
    return &unk_00868F10;
}

} /* extern "C" */
