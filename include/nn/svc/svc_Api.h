#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/dbg/dbg_Types.h>
#include <nn/os/os_Types.h>

#ifdef __ARMCC_VERSION
#define NN_SVC_ASM __asm
#else
#define NN_SVC_ASM
#endif

namespace nn {
namespace svc {
    NN_SVC_ASM nn::Result ControlMemory(u32* addr, u32 addr0, u32 addr1, u32 size, u32 op, u32 perms);                                /* 0x0018A784 */
    NN_SVC_ASM void ExitProcess();                                                                                                     /* 0x00104778 */
    NN_SVC_ASM void SleepThread(u64 nanos);                                                                                            /* 0x00151FBC */
    NN_SVC_ASM nn::Result GetThreadPriority(s32* prio, nn::Handle thread);
    NN_SVC_ASM nn::Result CreateMutex(nn::Handle* mutex, bool initialLocked);
    NN_SVC_ASM nn::Result CreateEvent(nn::Handle* event, nn::os::ResetType type);                                                     /* 0x0010AAA8 */
    NN_SVC_ASM nn::Result CreateMemoryBlock(nn::Handle* memblock, u32 addr, u32 size, u32 myperms, u32 otherperms);                  /* 0x0018A738 */
    NN_SVC_ASM nn::Result CreateAddressArbiter(nn::Handle* arbiter);                                                                 /* 0x00151FA0 */
    NN_SVC_ASM nn::Result ArbitrateAddress(nn::Handle arbiter, uintptr_t addr, nn::os::ArbitrationType type, s32 value, s64 ns);        /* 0x00166948 */
    NN_SVC_ASM nn::Result CloseHandle(nn::Handle handle);                                                                              /* 0x0018A714 */
    NN_SVC_ASM nn::Result WaitSynchronizationN(int* out, const nn::Handle* handles, s32 handleCount, bool waitAll, s64 timeout_ns);  /* 0x0013EC64 */
    NN_SVC_ASM nn::Result DuplicateHandle(nn::Handle* out, nn::Handle original);                                                      /* 0x0010AAC0 */
    NN_SVC_ASM nn::Result ConnectToPort(nn::Handle* port, const char* portName);
    NN_SVC_ASM nn::Result GetProcessId(unsigned int* id, nn::Handle handle);
    NN_SVC_ASM nn::Result GetThreadId(unsigned int* id, nn::Handle handle);                                                           /* 0x00151FC4 */
    NN_SVC_ASM nn::Result GetResourceLimit(Handle* resourceLimit, nn::Handle handle);

    // Debug
    NN_SVC_ASM nn::Result Break(nn::dbg::BreakReason reason);                                                                          /* 0x0013EC48 */

} // namespace svc
} // namespace nn
