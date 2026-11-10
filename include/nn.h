/**
 * @file nn.h
 * @brief Master CTR-SDK Header (`nn`).
 *
 * Provides declarative access to the full Nintendo 3DS CTR-SDK
 * architecture following the community standard layout (ACNLWA / 3dsdecomp).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Result.h"
#include "nn/Handle.h"

/* Operating System, Synchronization & Supervisor Calls */
#include "nn/os.h"
#include "nn/svc.h"
#include "nn/srv.h"

/* Core System & Foundation Libraries */
#include "nn/fnd.h"
#include "nn/math.h"
#include "nn/dbg.h"
#include "nn/err.h"
#include "nn/cfg.h"
#include "nn/ptm.h"

/* System Applets & Storage */
#include "nn/applet.h"
#include "nn/fs.h"

/* User Input & Hardware Interfaces */
#include "nn/hid.h"
#include "nn/gsp.h"
#include "nn/dsp.h"
#include "nn/snd.h"
#include "nn/cam.h"
#include "nn/mic.h"
#include "nn/y2r.h"

/* Networking & Online Daemons */
#include "nn/ac.h"
#include "nn/ndm.h"
#include "nn/cecd.h"
#include "nn/boss.h"
#include "nn/nex.h"
