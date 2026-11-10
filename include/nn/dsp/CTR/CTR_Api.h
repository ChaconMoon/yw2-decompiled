#pragma once

#include "types.h"
#include "nn/Handle.h"
#include "nn/Result.h"

namespace nn {
namespace dsp {
namespace CTR {

// A callback of RegisterSleepWakeUpCallback
typedef void (*SleepWakeUpCallback)(void);

nn::Result Initialize(void);
nn::Result Finalize(void);

// Components: the program the DSP runs.
nn::Result LoadDefaultComponent(void);
nn::Result LoadComponent(const u8 *component, u32 size, u16 programMask, u16 dataMask);
nn::Result UnloadComponent(void);
bool IsComponentLoaded(void); /* 0x001389BC */

// Readers of the component (against unloading)
void LockComponent(void);                                                                                       /* 0x00138A90 */
void UnlockComponent(void);                                                                                     /* 0x00138A80 */

// Wrappers of the DSP commands
nn::Result SetSemaphore(u16 value);                                                                             /* 0x0013862C */
nn::Result RecvDataIsReady(u16 registerNumber, bool *pIsReady);                                                  /* 0x001387E4 */
nn::Result RecvData(u16 registerNumber, u16 *pData);                                                            /* 0x00138688 */
nn::Result WriteProcessPipe(int channel, const void *buffer, unsigned size);                                    /* 0x001385EC */
nn::Result ReadPipeIfPossible(int channel, void *buffer, unsigned short size, unsigned short *pReadSize);       /* 0x00138880 */
nn::Result SetSemaphoreMask(u16 mask);                                                                          /* 0x00138824 */
nn::Result GetSemaphoreEventHandle(nn::Handle *pEvent);                                                          /* 0x001388CC */
nn::Result RegisterInterruptEvents(nn::Handle event, int interrupt, int channel);                               /* 0x001386EC */
nn::Result ConvertProcessAddressFromDspDram(unsigned int address, unsigned int *pAddress);                      /* 0x00138A30 */
nn::Result FlushDataCache(unsigned address, unsigned size);                                                     /* 0x00138660 */

// Callbacks: called before component unload for sleep, after load, and by OrderToWaitForFinalize
bool RegisterSleepWakeUpCallback(SleepWakeUpCallback sleep, SleepWakeUpCallback wakeUp, SleepWakeUpCallback waitForFinalize);
bool ClearSleepWakeUpCallback(SleepWakeUpCallback sleep, SleepWakeUpCallback wakeUp, SleepWakeUpCallback waitForFinalize);
void Sleep(void);
void WakeUp(void);
void Awake(void);
void OrderToWaitForFinalize(void);

} // namespace CTR
} // namespace dsp
} // namespace nn
