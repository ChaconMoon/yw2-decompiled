#pragma once

#include "types.h"
#include "nn/Handle.h"
#include "nn/Result.h"

namespace nn {
namespace dsp {
namespace CTR {

/**
 * @brief The commands of the service dsp::DSP (3dbrew "DSP Services").
 * Holds the session handle at offset 0x0.
 */
class DSP
{
public:
    nn::Result RecvData(u16 registerNumber, u16 *pData);
    nn::Result RecvDataIsReady(u16 registerNumber, bool *pIsReady);
    nn::Result SetSemaphore(u16 value);
    nn::Result ConvertProcessAddressFromDspDram(u32 address, u32 *pAddress);
    nn::Result WriteProcessPipe(s32 channel, const u8 *buffer, u32 size);
    nn::Result ReadPipeIfPossible(s32 channel, s32 peer, u8 *buffer, u16 size, u16 *pReadSize);
    nn::Result LoadComponent(const u8 *component, u32 size, u16 programMask, u16 dataMask, bool *pIsLoaded);
    nn::Result UnloadComponent();
    nn::Result FlushDataCache(nn::Handle process, u32 address, u32 size);
    nn::Result RegisterInterruptEvents(nn::Handle event, s32 interrupt, s32 channel);
    nn::Result GetSemaphoreEventHandle(nn::Handle *pEvent);
    nn::Result SetSemaphoreMask(u16 mask);
    nn::Result ForceHeadphoneOut(bool isForced);

    nn::Handle m_Session; // 0x00
};

} // namespace CTR
} // namespace dsp
} // namespace nn
