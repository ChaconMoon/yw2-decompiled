/**
 * @file os.h
 * @brief Nintendo CTR-SDK Operating System Library (`nn::os`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Handle.h"
#include "nn/Result.h"
#include "nn/os/os_Api.h"

#ifdef __cplusplus

#include "nn/os/os_Types.h"
#include "nn/os/os_HandleObject.h"
#include "nn/os/os_WaitObject.h"
#include "nn/os/os_InterruptEvent.h"
#include "nn/os/os_EventBase.h"
#include "nn/os/os_KConfigMemory.h"

namespace nn {
namespace os {

    typedef s64 Tick;

    class TimeSpan {
    private:
        s64 m_NanoSeconds;

    public:
        TimeSpan() : m_NanoSeconds(0) {}
        explicit TimeSpan(s64 ns) : m_NanoSeconds(ns) {}

        static TimeSpan FromNanoSeconds(s64 ns)   { return TimeSpan(ns); }
        static TimeSpan FromMicroSeconds(s64 us)  { return TimeSpan(us * 1000); }
        static TimeSpan FromMilliSeconds(s64 ms)  { return TimeSpan(ms * 1000000); }
        static TimeSpan FromSeconds(s64 s)        { return TimeSpan(s * 1000000000); }

        s64 GetNanoSeconds() const  { return m_NanoSeconds; }
        s64 GetMicroSeconds() const { return m_NanoSeconds / 1000; }
        s64 GetMilliSeconds() const { return m_NanoSeconds / 1000000; }
        s64 GetSeconds() const      { return m_NanoSeconds / 1000000000; }
    };

    struct CriticalSection {
        u32 m_Lock;
        u32 m_ThreadId;
        s32 m_Counter;
    };

    class Mutex : public HandleObject {
    public:
        Mutex();
        ~Mutex();
        void Lock();
        void Unlock();
        bool TryLock();
    };

    class Event : public EventBase {
    public:
        Event();
        Event(ResetType resetType);
        ~Event();
        void Signal();
        void Clear();
        void Wait();
        bool TryWait();
    };

    struct LightEvent {
        vu32 m_State;
    };

    struct LightSemaphore {
        s32 m_Count;
        s32 m_MaxCount;
        vu32 m_WaitThreads;
    };

    typedef void (*ThreadFunc)(uintptr_t arg);

    struct Thread {
        Handle m_Handle;
        void*  m_StackTop;
        u32    m_StackSize;
        s32    m_Priority;
    };

    /* System Tick & Time Functions */
    Tick GetSystemTick();                                /* 0x005F015C */
    s64  TickToTimeSpan(Tick tick);
    Tick TimeSpanToTick(s64 span_ns);
    s64  ConvertTickToMicroSeconds(Tick tick);
    s64  ConvertTickToMilliSeconds(Tick tick);

    /* Thread Management */
    Result CreateThread(Thread* thread, ThreadFunc entry, uintptr_t arg, void* stack, u32 stackSize, s32 priority, s32 coreId); /* 0x0018A750 */
    void   StartThread(Thread* thread);
    void   ExitThread();                                 /* 0x0012F190 */
    void   SleepThread(s64 nanos);                       /* 0x00151FBC */
    void   SleepThread(TimeSpan span);
    Handle GetCurrentThread();                           /* 0x001275E4 */
    s32    GetCurrentCoreId();                           /* 0x00127520 */
    s32    GetThreadPriority(Handle thread);
    void   SetThreadPriority(Handle thread, s32 priority);

    /* Critical Section Primitives */
    void InitializeCriticalSection(CriticalSection* cs); /* 0x00127200 */
    void EnterCriticalSection(CriticalSection* cs);      /* 0x001272E8 */
    void LeaveCriticalSection(CriticalSection* cs);      /* 0x001273CC */
    bool TryEnterCriticalSection(CriticalSection* cs);   /* 0x0012746C */

    /* Mutex Primitives */
    void InitializeMutex(Mutex* mutex, bool initialLocked);
    void FinalizeMutex(Mutex* mutex);
    void LockMutex(Mutex* mutex);
    void UnlockMutex(Mutex* mutex);
    bool TryLockMutex(Mutex* mutex);

    /* Event Primitives */
    void InitializeEvent(Event* event, ResetType resetType); /* 0x0010AAA8 */
    void FinalizeEvent(Event* event);                        /* 0x0018A714 */
    void SignalEvent(Event* event);
    void ClearEvent(Event* event);
    void WaitEvent(Event* event);
    bool TryWaitEvent(Event* event);

    /* Memory / Heap Info */
    u32  GetAppMemoryAllocatableSize();                  /* 0x00106504 */

} // namespace os
} // namespace nn

#else /* !__cplusplus (Pure C) */

typedef s64 nn_os_Tick;

typedef struct nn_os_CriticalSection {
    u32 m_Lock;
    u32 m_ThreadId;
    s32 m_Counter;
} nn_os_CriticalSection;

typedef struct nn_os_Thread {
    u32   m_Handle;
    void* m_StackTop;
    u32   m_StackSize;
    s32   m_Priority;
} nn_os_Thread;

nn_os_Tick nn_os_GetSystemTick(void);
void       nn_os_SleepThread(s64 nanos);
s32        nn_os_GetCurrentCoreId(void);

#endif /* __cplusplus */
