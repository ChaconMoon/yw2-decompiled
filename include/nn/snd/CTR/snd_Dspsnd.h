#ifndef NN_SND_CTR_DSPSND_H_
#define NN_SND_CTR_DSPSND_H_

#include "types.h"
#include "ctr/dsp.h"

#ifdef __cplusplus
extern "C" {
#endif

void thunk_FUN_0013457c(void *arg0);
void func_00136318(DspHeader_00136318 *header);
DspObject_00136318* func_00136330(DspObject_00136318 *self);
void func_0013636C(void *arg0);
void func_00136420(DspInterface_00136420 *obj);
DspChannel_00136804 *func_00136804(DspChannel_00136804 *obj);
void func_001375DC(DspComponent_001375DC *obj);
u16  func_00137920(DspChannel_00137920 *channel, u16 val);
u16  func_00137930(DspChannel_00137920 *channel, u16 flag);
s8   func_00137A0C(DspChannel_00137920 *channel, s8 state);
void func_00137F18(void *param);
bool32 func_00137F28(DspPair_00137F28 *obj, void *param1, void *param2);
u32  func_00137F40(DspBuffer_00137F40 *obj);
bool32 func_0013810C(DspPair_00137F28 *obj, void *param1, void *param2);

#ifdef __cplusplus
}
#endif

#endif // NN_SND_CTR_DSPSND_H_
