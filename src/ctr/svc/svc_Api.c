/**
 * @file svc_Api.c
 * @brief Nintendo 3DS Kernel Supervisor Call (SVC) API wrappers (`nn::svc`).
 */

#include <ctr/svc.h>

/**
 * @brief svcExitProcess
 * @address 0x00104778
 * CTR kernel syscall wrapper stub (svc 0x03)
 */
void svcExitProcess(void)
{
    __asm__ volatile("svc 0x00000003");
}

/**
 * @brief nn::svc::ControlMemory (func_0010AA84)
 * @address 0x0010AA84
 * CTR kernel syscall wrapper for svc 0x01.
 * Controls application virtual memory allocation, mapping, and page permissions.
 * @param outAddr Pointer receiving the allocated/modified virtual address.
 * @param addr0 Primary target address.
 * @param addr1 Secondary target address (for aliasing or mirroring).
 * @param size Size of the memory region in bytes (aligned to page size 0x1000).
 * @param op Memory operation flags (commit, free, reserve).
 * @param perm Memory permission flags (read, write, execute).
 * @return Kernel Result code.
 */
__attribute__((naked))
Result
func_0010AA84(u32 *outAddr, u32 addr0, u32 addr1, u32 size, u32 op, u32 perm)
{
    __asm__ volatile(
        "push {r0, r4}\n"
        "ldr r0, [sp, #8]\n"
        "ldr r4, [sp, #12]\n"
        "svc 0x00000001\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "pop {r4}\n"
        "bx lr");
}

/**
 * @brief svcCreateEvent
 * @address 0x0010AAA8
 * CTR kernel syscall wrapper stub (svc 0x21)
 */
__attribute__((naked))
Result
svcCreateEvent(Handle *event, u32 resetType)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x00000021\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief svcDuplicateHandle
 * @address 0x0010AAC0
 * CTR kernel syscall wrapper stub (svc 0x38)
 */
__attribute__((naked))
Result
svcDuplicateHandle(Handle *handle, Handle original)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x00000038\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief svcCreateTimer
 * @address 0x00112FF0
 * CTR kernel syscall wrapper stub (svc 0x2D)
 */
__attribute__((naked))
Result
svcCreateTimer(Handle *timer, u32 resetType)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x0000002d\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief nn::svc::CreateThread (func_00113008)
 * @address 0x00113008
 * CTR kernel syscall wrapper for svc 0x08.
 * Spawns a new execution thread within the calling process.
 * @param thread Pointer receiving the created thread kernel Handle.
 * @param entrypoint Thread entrypoint function pointer.
 * @param arg User argument passed to the thread entrypoint.
 * @param stackTop Pointer to the top of the allocated thread stack memory.
 * @param priority Thread scheduling priority (0x18 - 0x3F).
 * @param processorId Target CPU core ID (0 = Core0 / AppCore, 1 = Core1 / SysCore, -1 = default).
 * @return Kernel Result code.
 */
__attribute__((naked))
Result
func_00113008(Handle *thread, ThreadFunc entrypoint, u32 arg, u32 *stackTop, s32 priority, s32 processorId)
{
    __asm__ volatile(
        "push {r0, r4}\n"
        "ldr r0, [sp, #8]\n"
        "ldr r4, [sp, #12]\n"
        "svc 0x00000008\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "pop {r4}\n"
        "bx lr");
}

/**
 * @brief svcExitThread
 * @address 0x0012F190
 * CTR kernel syscall wrapper stub (svc 0x09)
 */
void svcExitThread(void)
{
    __asm__ volatile("svc 0x00000009");
}

/**
 * @brief svcCreateResourceLimit
 * @address 0x0012F198
 * CTR kernel syscall wrapper stub (svc 0x35)
 */
__attribute__((naked))
Result
svcCreateResourceLimit(Handle *resourceLimit)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x00000035\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief nn::svc::BreakReason4 (func_0013EC38)
 * @address 0x0013EC38
 * CTR kernel syscall wrapper for svc 0x3C with panic reason code #4.
 * Halts execution and triggers the CTR system panic handler.
 * Preserves ARM instruction alignment with an explicit 'mov r0, r0'.
 * @param arg0 First debug/diagnostic argument.
 * @param arg1 Second debug/diagnostic argument.
 */
__attribute__((naked))
void func_0013EC38(u32 arg0, u32 arg1)
{
    __asm__ volatile(
        "mov r2, r1\n"
        "mov r1, r0\n"
        "mov r0, #4\n"
        "mov r0, r0\n"
        "svc 0x0000003c\n"
        "bx lr");
}

/**
 * @brief svcBreak
 * @address 0x0013EC48
 * CTR kernel syscall wrapper stub (svc 0x3C)
 */
void svcBreak(u32 reason)
{
    __asm__ volatile("svc 0x0000003C");
}

/**
 * @brief func_0013EC50
 * @address 0x0013EC50
 * Thread yield stub via svc 0x0A (svcSleepThread(0))
 */
__attribute__((naked)) void func_0013EC50(void)
{
    __asm__ volatile(
        "mov r0, #0\n"
        "mov r1, r0\n"
        "mov r0, r0\n"
        "svc 0x0000000a\n"
        "bx lr");
}

/**
 * @brief func_0013EC5C
 * @address 0x0013EC5C
 * CTR kernel syscall wrapper stub (svc 0x0A)
 */
void func_0013EC5C(s64 nanoseconds)
{
    __asm__ volatile("svc 0x0000000A");
}

/**
 * @brief nn::svc::WaitSynchronizationN (svcWaitSynchronizationN)
 * @address 0x0013EC64
 * CTR kernel syscall wrapper for svc 0x25.
 * Waits until one or all of the specified synchronization handles are signaled.
 * @param outIndex Pointer receiving the index of the handle that satisfied the wait condition.
 * @param handles Array of kernel synchronization handles to wait on.
 * @param handleCount Number of handles in the array (max 64).
 * @param waitAll If true, waits until all handles are signaled; if false, any handle wakes the caller.
 * @param timeout Maximum wait duration in nanoseconds.
 * @return Kernel Result code.
 */
__attribute__((naked))
Result
svcWaitSynchronizationN(s32 *outIndex, const Handle *handles, s32 handleCount, bool waitAll, s64 timeout)
{
    __asm__ volatile(
        "push {r0, r4}\n"
        "ldr r0, [sp, #8]\n"
        "ldr r4, [sp, #12]\n"
        "svc 0x00000025\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "pop {r4}\n"
        "bx lr");
}

/**
 * @brief svcCreateAddressArbiter
 * @address 0x00151FA0
 * CTR kernel syscall wrapper stub (svc 0x17)
 */
__attribute__((naked))
Result
svcCreateAddressArbiter(Handle *arbiter)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x00000017\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief svcSleepThread
 * @address 0x00151FBC
 * CTR kernel syscall wrapper stub (svc 0x28)
 */
void svcSleepThread(s64 nanoseconds)
{
    __asm__ volatile("svc 0x00000028");
}

/**
 * @brief svcGetThreadId
 * @address 0x00151FC4
 * CTR kernel syscall wrapper stub (svc 0x37)
 */
__attribute__((naked))
Result
svcGetThreadId(u32 *threadId, Handle thread)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x00000037\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief nn::svc::ArbitrateAddress (svcArbitrateAddress)
 * @address 0x00166948
 * CTR kernel syscall wrapper for svc 0x22.
 * Synchronizes threads using an address arbiter object for fast user-space mutexes
 * and condition variables.
 * @param arbiter Handle to the address arbiter.
 * @param addr Virtual address to arbitrate on.
 * @param type Arbitration operation type (signal, wait, etc.).
 * @param value Comparison value for conditional wait.
 * @param timeout Timeout in nanoseconds (<= 0 for no timeout).
 * @return Kernel Result code.
 */
__attribute__((naked))
Result
svcArbitrateAddress(Handle arbiter, u32 addr, u32 type, s32 value, s64 timeout)
{
    __asm__ volatile(
        "push {r4, r5}\n"
        "ldr r4, [sp, #8]\n"
        "ldr r5, [sp, #12]\n"
        "svc 0x00000022\n"
        "pop {r4, r5}\n"
        "bx lr");
}

/**
 * @brief svcCloseHandle
 * @address 0x0018A714
 * CTR kernel syscall wrapper stub (svc 0x23)
 */
Result svcCloseHandle(Handle handle)
{
    __asm__ volatile("svc 0x00000023");
}

/**
 * @brief nn::svc::CreateMemoryBlock (func_0018A71C)
 * @address 0x0018A71C
 * CTR kernel syscall wrapper for svc 0x1E.
 * Creates a shared memory block object that can be mapped into multiple processes.
 * @param memoryBlock Pointer receiving the created memory block Handle.
 * @param addr Base virtual address for the memory block.
 * @param size Size of the memory block in bytes.
 * @param myPermission Permissions granted to the creating process.
 * @param otherPermission Permissions granted to other processes mapping the block.
 * @return Kernel Result code.
 */
__attribute__((naked))
Result
func_0018A71C(Handle *memoryBlock, u32 addr, u32 size, u32 myPermission, u32 otherPermission)
{
    __asm__ volatile(
        "push {r0}\n"
        "ldr r0, [sp, #4]\n"
        "svc 0x0000001e\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief svcCreateMemoryBlock
 * @address 0x0018A738
 * CTR kernel syscall wrapper stub (svc 0x13)
 */
__attribute__((naked))
Result
svcCreateMemoryBlock(Handle *memblock, u32 addr, u32 size, u32 myPerm, u32 otherPerm)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x00000013\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief svcCreateThread
 * @address 0x0018A750
 * CTR kernel syscall wrapper stub (svc 0x27)
 */
__attribute__((naked))
Result
svcCreateThread(Handle *thread, ThreadFunc entrypoint, u32 arg, u32 *stackTop, s32 priority, s32 processorId)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x00000027\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief nn::svc::GetHandleInfo (svcGetHandleInfo)
 * @address 0x0018A768
 * CTR kernel syscall wrapper for svc 0x29.
 * Queries diagnostic information and properties about a kernel handle.
 * @param out Pointer receiving the 64-bit handle information.
 * @param handle Target kernel handle.
 * @param type Information query sub-type.
 * @return Kernel Result code.
 */
__attribute__((naked))
Result
svcGetHandleInfo(s64 *out, Handle handle, u32 type)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x00000029\n"
        "ldr r3, [sp]\n"
        "str r1, [r3]\n"
        "str r2, [r3, #4]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief svcControlMemory
 * @address 0x0018A784
 * CTR kernel syscall wrapper stub (svc 0x0B)
 */
__attribute__((naked))
Result
svcControlMemory(u32 *addrOut, u32 addr0, u32 addr1, u32 size, u32 op, u32 perm)
{
    __asm__ volatile(
        "push {r0}\n"
        "svc 0x0000000b\n"
        "ldr r2, [sp]\n"
        "str r1, [r2]\n"
        "add sp, sp, #4\n"
        "bx lr");
}

/**
 * @brief svcWaitSynchronization1
 * @address 0x0018A868
 * CTR kernel syscall wrapper stub (svc 0x24)
 */
Result svcWaitSynchronization1(Handle handle, s64 nanoseconds)
{
    __asm__ volatile("svc 0x00000024");
}

/**
 * @brief nn::svc::GetSystemTickToPtr (func_005F015C)
 * @address 0x005F015C
 * Invokes CTR kernel syscall svc 0x28 (GetSystemTick) to retrieve the 64-bit hardware
 * timer tick and stores the 64-bit value into the supplied pointer via strd.
 * @param outTick Pointer to an s64 variable that receives the system tick.
 * @return Returns the destination pointer outTick.
 */
__attribute__((naked))
s64 *func_005F015C(s64 *outTick)
{
    __asm__ volatile(
        "push {r4, lr}\n"
        "mov r4, r0\n"
        "svc 0x00000028\n"
        "strd r0, [r4]\n"
        "mov r0, r4\n"
        "pop {r4, pc}");
}
