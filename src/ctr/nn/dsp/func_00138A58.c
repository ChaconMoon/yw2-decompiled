#include "ctr/dsp.h"
#include "ctr/svc.h"

/**
 * @brief Sends a synchronous IPC request to the DSP service (command 0x001A0000 - SetIirFilterI2S1).
 *
 * Address: 0x00138A58
 * Size:    40 bytes
 *
 * @param pHandle Pointer to the service session handle.
 * @return Result code from svcSendSyncRequest, or response value from cmdbuf[1] on success.
 */
s32 func_00138A58(const u32 *pHandle)
{
	u32 *cmdbuf;
	s32 res;

	/* Retrieve TLS base pointer from ARM11 CP15 coprocessor */
	CTR_GET_TLS(cmdbuf);

	/* Access IPC Command Buffer located at TLS offset +0x80 (128 bytes) */
	cmdbuf = (u32 *)((char *)cmdbuf + 128);

	/* Write IPC command header 0x001A0000 (Command ID: 0x001A - SetIirFilterI2S1, 0 normal words, 0 descriptors) */
	*cmdbuf = 0x001A0000;

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
