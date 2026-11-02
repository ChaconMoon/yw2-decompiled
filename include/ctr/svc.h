#ifndef CTR_SVC_H
#define CTR_SVC_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

        typedef u32 Handle;
        typedef s32 Result;
        typedef void (*ThreadFunc)(u32 arg);

        /* Nintendo 3DS Kernel Supervisor Calls (SVC) */
        void svcExitProcess(void);                                                                                            /* 0x00104778 (svc 0x03) */
        Result func_0010AA84(u32 *outAddr, u32 addr0, u32 addr1, u32 size, u32 op, u32 perm);                                 /* 0x0010AA84 (svc 0x01) */
        Result svcCreateEvent(Handle *event, u32 resetType);                                                                  /* 0x0010AAA8 (svc 0x21) */
        Result svcDuplicateHandle(Handle *handle, Handle original);                                                           /* 0x0010AAC0 (svc 0x38) */
        Result svcCreateTimer(Handle *timer, u32 resetType);                                                                  /* 0x00112FF0 (svc 0x2D) */
        Result func_00113008(Handle *thread, ThreadFunc entrypoint, u32 arg, u32 *stackTop, s32 priority, s32 processorId);   /* 0x00113008 (svc 0x08) */
        void svcExitThread(void);                                                                                             /* 0x0012F190 (svc 0x09) */
        Result svcCreateResourceLimit(Handle *resourceLimit);                                                                 /* 0x0012F198 (svc 0x35) */
        void func_0013EC38(u32 arg0, u32 arg1);                                                                               /* 0x0013EC38 (svc 0x3C) */
        void svcBreak(u32 reason);                                                                                            /* 0x0013EC48 (svc 0x3C) */
        void func_0013EC50(void);                                                                                             /* 0x0013EC50 (svc 0x0A) */
        void func_0013EC5C(s64 nanoseconds);                                                                                  /* 0x0013EC5C (svc 0x0A) */
        Result svcWaitSynchronizationN(s32 *outIndex, const Handle *handles, s32 handleCount, bool waitAll, s64 timeout);     /* 0x0013EC64 (svc 0x25) */
        Result svcCreateAddressArbiter(Handle *arbiter);                                                                      /* 0x00151FA0 (svc 0x17) */
        void svcSleepThread(s64 nanoseconds);                                                                                 /* 0x00151FBC (svc 0x28) */
        Result svcGetThreadId(u32 *threadId, Handle thread);                                                                  /* 0x00151FC4 (svc 0x37) */
        Result svcArbitrateAddress(Handle arbiter, u32 addr, u32 type, s32 value, s64 timeout);                               /* 0x00166948 (svc 0x22) */
        Result svcCloseHandle(Handle handle);                                                                                 /* 0x0018A714 (svc 0x23) */
        Result func_0018A71C(Handle *memoryBlock, u32 addr, u32 size, u32 myPermission, u32 otherPermission);                 /* 0x0018A71C (svc 0x1E) */
        Result svcCreateMemoryBlock(Handle *memblock, u32 addr, u32 size, u32 myPerm, u32 otherPerm);                         /* 0x0018A738 (svc 0x13) */
        Result svcCreateThread(Handle *thread, ThreadFunc entrypoint, u32 arg, u32 *stackTop, s32 priority, s32 processorId); /* 0x0018A750 (svc 0x27) */
        Result svcGetHandleInfo(s64 *out, Handle handle, u32 type);                                                           /* 0x0018A768 (svc 0x29) */
        Result svcControlMemory(u32 *addrOut, u32 addr0, u32 addr1, u32 size, u32 op, u32 perm);                              /* 0x0018A784 (svc 0x0B) */
        Result svcWaitSynchronization1(Handle handle, s64 nanoseconds);                                                       /* 0x0018A868 (svc 0x24) */
        s64 *func_005F015C(s64 *outTick);                                                                                     /* 0x005F015C (svc 0x28) */

#ifdef __ARMCC_VERSION
        u32 __svc(0x23) svcCloseHandle_inline(Handle handle);
        s32 __svc(0x32) svcSendSyncRequest_inline(Handle session);
#else
        u32 svcCloseHandle_inline(Handle handle);
        s32 svcSendSyncRequest_inline(Handle session);
#endif

#ifdef __cplusplus
}
#endif

#endif /* CTR_SVC_H */
