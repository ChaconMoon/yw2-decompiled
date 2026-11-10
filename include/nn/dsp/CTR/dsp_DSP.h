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
    nn::Result RecvData(u16 registerNumber, u16 *pData);                                                /* 0x00138688 */
    nn::Result RecvDataIsReady(u16 registerNumber, bool *pIsReady);                                     /* 0x001387E4 */
    nn::Result SetSemaphore(u16 value);                                                                /* 0x0013862C */
    nn::Result ConvertProcessAddressFromDspDram(u32 address, u32 *pAddress);                             /* 0x00138A30 */
    nn::Result WriteProcessPipe(s32 channel, const u8 *buffer, u32 size);                               /* 0x001385EC */
    nn::Result ReadPipeIfPossible(s32 channel, s32 peer, u8 *buffer, u16 size, u16 *pReadSize);        /* 0x00138880 */
    nn::Result LoadComponent(const u8 *component, u32 size, u16 programMask, u16 dataMask, bool *pIsLoaded);
    nn::Result UnloadComponent();
    nn::Result FlushDataCache(nn::Handle process, u32 address, u32 size);                               /* 0x00138660 */
    nn::Result RegisterInterruptEvents(nn::Handle event, s32 interrupt, s32 channel);                  /* 0x00138A58 */
    nn::Result GetSemaphoreEventHandle(nn::Handle *pEvent);                                             /* 0x00138858 */
    nn::Result SetSemaphoreMask(u16 mask);                                                             /* 0x00138824 */
    nn::Result ForceHeadphoneOut(bool isForced);

    nn::Handle m_Session; // 0x00
};

} // namespace CTR
} // namespace dsp
} // namespace nn
