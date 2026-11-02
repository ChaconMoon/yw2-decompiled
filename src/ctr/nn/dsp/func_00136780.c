#include "ctr/dsp.h"

/**
 * @brief Sets a value in the Thread Local Storage (TLS) array.
 *
 * Address: 0x00136780
 * Size:    16 bytes
 *
 * @param pSlot Pointer to the allocated TLS slot index.
 * @param value Value to store in the thread-local storage slot.
 */
void func_00136780(const u32 *pSlot, u32 value)
{
        u32 *tls;
        __asm {
                mrc p15, 0, tls, c13, c0, 3
        }
        tls[*pSlot] = value;
}
