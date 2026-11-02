#include "ctr/dsp.h"
#include "ctr/svc.h"

/**
 * @brief Sends a synchronous IPC request to the DSP service to write multi-byte registers to SPI2 (Instance 2).
 *
 * Command ID: 0x001E (Header: 0x001E0042, 1 normal parameter word, 1 buffer translation descriptor)
 * Address:    0x001387E4
 * Size:       60 bytes
 *
 * @param pHandle Pointer to the service session handle.
 * @param buffer Pointer to the input buffer containing data to write.
 * @param size Size in bytes of the buffer to write.
 * @return Result code from svcSendSyncRequest on failure, or response word from cmdbuf[1] on success.
 */
s32 func_001387E4(const u32 *pHandle, const void *buffer, u32 size)
{
	u32 *cmdbuf;
	s32 res;

	/* Retrieve TLS base pointer from ARM11 CP15 coprocessor */
	CTR_GET_TLS(cmdbuf);

	/* Access IPC Command Buffer located at TLS offset +0x80 (128 bytes) */
	cmdbuf = (u32 *)((char *)cmdbuf + 128);

	/* Write IPC command header 0x001E0042 (Command ID 0x001E - WriteMultiEx_SPI2) */
	cmdbuf[0] = 0x001E0042;

	__schedule_barrier();

	/* Parameter 1: Buffer size */
	cmdbuf[1] = size;

	/* Descriptor: Static buffer translation descriptor ((size << 4) | 0xA) */
	cmdbuf[2] = (10 | size << 4);

	/* Static buffer address pointer */
	cmdbuf[3] = (u32)buffer;

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
