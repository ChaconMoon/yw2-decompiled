#include "ctr/dsp.h"
#include "ctr/svc.h"

/**
 * @brief Sends a synchronous IPC request to the DSP service to mask and query the semaphore state (Instance 2).
 *
 * Command ID: 0x000A (Header: 0x000A0000, 0 normal words, 0 descriptors)
 * Address:    0x00138824
 * Size:       52 bytes
 *
 * @param pHandle Pointer to the service session handle.
 * @param pOutSemaphore Output pointer to receive the retrieved 8-bit semaphore byte (cmdbuf[2]).
 * @return Result code from svcSendSyncRequest on failure, or response word from cmdbuf[1] on success.
 */
s32 func_00138824(const u32 *pHandle, u8 *pOutSemaphore)
{
	u32 *cmdbuf;
	s32 res;

	/* Retrieve TLS base pointer from ARM11 CP15 coprocessor */
	CTR_GET_TLS(cmdbuf);

	/* Access IPC Command Buffer located at TLS offset +0x80 (128 bytes) */
	cmdbuf = (u32 *)((char *)cmdbuf + 128);

	/* Write IPC command header 0x000A0000 (Command ID: 0x000A - MaskSemaphore) */
	*cmdbuf = 0x000A0000;

	/* Issue synchronous IPC request to the DSP sysmodule via session handle */
	res = svcSendSyncRequest_inline(*pHandle);

	/* Check if Result succeeded (bit 31 is 0 for success in CTR-SDK Result codes) */
	if ((s32)(res & 0x80000000) >= 0)
	{
		/* Store the retrieved 8-bit semaphore mask from cmdbuf[2] (byte offset +8) */
		*pOutSemaphore = ((u8 *)cmdbuf)[8];

		/* Retrieve response data placed by the DSP service in cmdbuf[1] */
		res = cmdbuf[1];
	}

	return res;
}
