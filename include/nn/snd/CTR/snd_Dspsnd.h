#ifndef NN_SND_CTR_DSPSND_H_
#define NN_SND_CTR_DSPSND_H_

#include "types.h"
#include "ctr/dsp.h"

#ifdef __cplusplus
extern "C" {
#endif

void thunk_FUN_0013457c(void *arg0);                                    /* 0x0013457C */
void func_00136318(DspHeader_00136318 *header);                         /* 0x00136318 */
DspObject_00136318* func_00136330(DspObject_00136318 *self);           /* 0x00136330 */
void func_0013636C(void *arg0);                                         /* 0x0013636C */
void func_00136420(DspInterface_00136420 *obj);                         /* 0x00136420 */
DspChannel_00136804 *func_00136804(DspChannel_00136804 *obj);          /* 0x00136804 */
void func_001375DC(DspComponent_001375DC *obj);                         /* 0x001375DC */
u16  func_00137920(DspChannel_00137920 *channel, u16 val);              /* 0x00137920 */
u16  func_00137930(DspChannel_00137920 *channel, u16 flag);             /* 0x00137930 */
s8   func_00137A0C(DspChannel_00137920 *channel, s8 state);             /* 0x00137A0C */
void func_00137F18(void *param);                                        /* 0x00137F18 */
bool32 func_00137F28(DspPair_00137F28 *obj, void *param1, void *param2); /* 0x00137F28 */
u32  func_00137F40(DspBuffer_00137F40 *obj);                            /* 0x00137F40 */
bool32 func_0013810C(DspPair_00137F28 *obj, void *param1, void *param2); /* 0x0013810C */

#ifdef __cplusplus
}
#endif

#endif // NN_SND_CTR_DSPSND_H_
