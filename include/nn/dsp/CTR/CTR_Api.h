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
bool IsComponentLoaded(void);

// Readers of the component (against unloading)
void LockComponent(void);
void UnlockComponent(void);

// Wrappers of the DSP commands
nn::Result SetSemaphore(u16 value);
nn::Result RecvDataIsReady(u16 registerNumber, bool *pIsReady);
nn::Result RecvData(u16 registerNumber, u16 *pData);
nn::Result WriteProcessPipe(int channel, const void *buffer, unsigned size);
nn::Result ReadPipeIfPossible(int channel, void *buffer, unsigned short size, unsigned short *pReadSize);
nn::Result SetSemaphoreMask(u16 mask);
nn::Result GetSemaphoreEventHandle(nn::Handle *pEvent);
nn::Result RegisterInterruptEvents(nn::Handle event, int interrupt, int channel);
nn::Result ConvertProcessAddressFromDspDram(unsigned int address, unsigned int *pAddress);
nn::Result FlushDataCache(unsigned address, unsigned size);

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
