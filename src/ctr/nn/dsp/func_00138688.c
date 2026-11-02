#include "ctr/dsp.h"
#include "ctr/svc.h"

/**
 * @brief Sends a synchronous IPC request to the DSP service to convert process address from DSP DRAM (Instance 1).
 *
 * Command ID: 0x000C (Header: 0x000C0082, 2 normal parameter words, 1 buffer translation descriptor)
 * Address:    0x00138688
 * Size:       72 bytes
 *
 * @param pHandle Pointer to the service session handle.
 * @param buffer Pointer to the input buffer.
 * @param size Size in bytes of the buffer.
 * @param flag 8-bit flag byte passed in cmdbuf[2].
 * @return Result code from svcSendSyncRequest on failure, or response word from cmdbuf[1] on success.
 */
s32 func_00138688(const u32 *pHandle, const void *buffer, u32 size, u8 flag)
{
	u32 *cmdbuf;
	s32 res;
	const u8 *pFlag = &flag;

	/* Retrieve TLS base pointer from ARM11 CP15 coprocessor */
	CTR_GET_TLS(cmdbuf);

	/* Access IPC Command Buffer located at TLS offset +0x80 (128 bytes) */
	cmdbuf = (u32 *)((char *)cmdbuf + 128);

	/* Write IPC command header 0x000C0082 (Command ID 0x000C - ConvertProcessAddressFromDspDram) */
	cmdbuf[0] = 0x000C0082;

	__schedule_barrier();

	/* Parameter 1: Buffer size */
	cmdbuf[1] = size;

	/* Parameter 2: 8-bit flag byte placed at byte offset +8 (cmdbuf[2]) */
	((u8 *)cmdbuf)[8] = *pFlag;

	__schedule_barrier();

	/* Descriptor: Static buffer translation descriptor ((size << 4) | 0xA) */
	cmdbuf[3] = (10 | size << 4);

	/* Static buffer address pointer */
	cmdbuf[4] = (u32)buffer;

	/* Issue synchronous IPC request to the DSP sysmodule via session handle */
	res = svcSendSyncRequest_inline(*pHandle);

	/* Check if Result succeeded (bit 31 is 0 for success in CTR-SDK Result codes) */
	if ((s32)(res & 0x80000000) >= 0)
	{
		/* On success, retrieve response data placed by the DSP service in cmdbuf[1] */
		res = cmdbuf[1];
	}

	return res;
}
