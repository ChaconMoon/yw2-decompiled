/**
 * @file os_Api.cpp
 * @brief Nintendo 3DS Operating System API (`nn::os`) implementation.
 */

#include <nn/os/os_Api.h>

extern "C" {

/**
 * @brief func_00104D18
 * @address 0x00104D18
 * nn::os startup global 8-bit flag setter (0x00814E54)
 */
void func_00104D18(u8 flag)
{
    g_flag_00814E54 = flag;
}

/**
 * @brief func_001063F4
 * @address 0x001063F4
 * CTR OS SystemConfig block field reader (0x1FF80000 + 0x40)
 */
u32 func_001063F4(void)
{
    return ((volatile u32 *)0x1FF80000)[0x10];
}

#ifdef __ARMCC_VERSION
#pragma push
#pragma Otime
#endif

/**
 * @brief func_00106504
 * @address 0x00106504
 * nn::os application heap memory region initialization (0x0E000000, 32 MB)
 */
void func_00106504(void)
{
    func_00112EF4(&unk_0086FC60, 0x0E000000, 0x02000000);
}

/**
 * @brief func_00106518
 * @address 0x00106518
 * nn::os heap memory region initialization (0x10000000, 64 MB)
 */
void func_00106518(void)
{
    func_00112EF4(&unk_0086FC48, 0x10000000, 0x04000000);
}

#ifdef __ARMCC_VERSION
#pragma pop
#endif

/**
 * @brief nn::os::detail::SetTlsStorage (func_0010655C)
 * @address 0x0010655C
 * Reads the ARM11 MPCore Thread-Local Storage (TLS) base address from system
 * coprocessor CP15 (c13, c0, 3 / TPIDRURO) and caches it into the global TLS
 * management structure at offset +0x04 (address 0x00815DE0 + 4).
 */
void func_0010655C(void)
{
    void *tls;
    CTR_GET_TLS(tls);
    *((void **)((char *)&g_tls_00815DE0 + 4)) = tls;
}

/**
 * @brief func_0010678C
 * @address 0x0010678C
 * nn::os startup global signed byte reader via literal pool (0x00814E00)
 */
s8 func_0010678C(void)
{
    return g_flag_00814E00;
}

/**
 * @brief func_00127520
 * @address 0x00127520
 * CTR-SDK .text size / limit getter via literal pool (0x005F32AC)
 */
u32 func_00127520(void)
{
    return (u32)&unk_005F32AC;
}

/**
 * @brief func_001275D8
 * @address 0x001275D8
 * CTR-SDK .text base VMA getter via literal pool (0x00100000)
 */
void *func_001275D8(void)
{
    return (void *)__text_start;
}

/**
 * @brief func_0012788C
 * @address 0x0012788C
 * nn::os global flag setter via literal pool (0x00828EEC)
 */
void func_0012788C(u8 val)
{
    unk_00828EEC = val;
}

} /* extern "C" */
