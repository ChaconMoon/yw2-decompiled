#ifndef ENGINE_LXP_H
#define ENGINE_LXP_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Level-5 Cross-Platform Engine (lxp)
 * lxpCMemory, memory pools, allocators, and container utilities.
 * ======================================================================== */

extern s32 unk_00868A9C;
extern s32 unk_00868F10;

void *func_0012CB00(void);         /* 0x0012CB00: returns &unk_00868A9C */
s32   func_0012FEC0(void);         /* 0x0012FEC0: returns 1 */
s32   func_00130018(void);         /* 0x00130018: returns 1 */
void *func_001300B0(void);         /* 0x001300B0: returns &unk_00868F10 */

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_LXP_H */
