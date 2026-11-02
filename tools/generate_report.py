#!/usr/bin/env python3
"""
tools/generate_report.py — Generates a comprehensive decompilation progress report
in the standard objdiff-cli format (version 2), compatible with decomp.dev and frogress.
"""

import csv
import json
from pathlib import Path
import re
import subprocess
import sys

PROJECT_ROOT = Path(__file__).resolve().parent.parent
CONFIG_DIR = PROJECT_ROOT / "config"
MANIFEST_CSV = CONFIG_DIR / "functions.csv"
CLASSIFIED_CSV = CONFIG_DIR / "functions_classified.csv"
MEMORY_MAP_JSON = CONFIG_DIR / "memory_map.json"
SRC_DIR = PROJECT_ROOT / "src"
BUILD_DIR = PROJECT_ROOT / "build"
REPORT_OUTPUT = BUILD_DIR / "report.json"

# Canonical Memory Map Constants (from ExHeader)
BASE_VMA = 0x00100000
TOTAL_CODE_BYTES = 6238892      # .text segment size
TOTAL_RODATA_BYTES = 1147948    # .rodata segment size
TOTAL_DATA_BYTES = 342324       # .data segment size
TOTAL_BSS_BYTES = 766088        # .bss segment size
TOTAL_ALL_DATA_BYTES = TOTAL_RODATA_BYTES + TOTAL_DATA_BYTES + TOTAL_BSS_BYTES  # 2,256,360 B
TOTAL_FUNCTIONS_EXPECTED = 18225

# Demangling cache for performance
_DEMANGLE_CACHE = {}

def demangle(sym: str) -> str:
    if sym in _DEMANGLE_CACHE:
        return _DEMANGLE_CACHE[sym]
    if sym.startswith("_Z"):
        try:
            res = subprocess.run(["c++filt", sym], capture_output=True, text=True, check=True)
            dem = res.stdout.strip()
            _DEMANGLE_CACHE[sym] = dem
            return dem
        except Exception:
            pass
    _DEMANGLE_CACHE[sym] = sym
    return sym


def load_all_functions():
    """Loads all functions, prioritizing config/functions.csv and completing with functions_classified.csv."""
    funcs_by_addr = {}

    # 1. Load cataloged functions (active in progress tracking)
    if MANIFEST_CSV.exists():
        with open(MANIFEST_CSV, "r", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                addr_hex = row.get("address", "").strip()
                if not addr_hex:
                    continue
                try:
                    addr_int = int(addr_hex, 16)
                    size = int(row.get("size", 0))
                except ValueError:
                    continue

                funcs_by_addr[addr_int] = {
                    "address": addr_int,
                    "address_hex": f"0x{addr_int:08X}",
                    "symbol": row.get("symbol", "").strip(),
                    "size": size,
                    "layer": row.get("layer", "game").strip().lower(),
                    "subsystem": row.get("subsystem", "core").strip(),
                    "status": row.get("status", "asm_stub").strip().lower(),
                    "description": row.get("description", "").strip()
                }

    # 2. Complete remaining functions from functions_classified.csv
    if CLASSIFIED_CSV.exists():
        with open(CLASSIFIED_CSV, "r", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                addr_hex = row.get("address", "").strip()
                if not addr_hex:
                    continue
                try:
                    addr_int = int(addr_hex, 16)
                    size = int(row.get("size", 0))
                except ValueError:
                    continue

                if addr_int not in funcs_by_addr:
                    funcs_by_addr[addr_int] = {
                        "address": addr_int,
                        "address_hex": f"0x{addr_int:08X}",
                        "symbol": row.get("symbol", "").strip() or f"func_{addr_int:08X}",
                        "size": size,
                        "layer": row.get("layer", "game").strip().lower(),
                        "subsystem": row.get("subsystem", "core").strip(),
                        "status": "asm_stub",
                        "description": row.get("description", "").strip()
                    }

    return sorted(funcs_by_addr.values(), key=lambda x: x["address"])


def find_source_files():
    """Maps symbols to their actual source files in src/."""
    file_map = {}
    if SRC_DIR.exists():
        for path in list(SRC_DIR.rglob("*.c")) + list(SRC_DIR.rglob("*.cpp")):
            stem = path.stem
            rel = str(path.relative_to(PROJECT_ROOT))
            file_map[stem] = rel
    return file_map


def build_measures_dict(total_code, matched_code, total_data, matched_data, total_fns, matched_fns, total_units, complete_units):
    matched_code_pct = (matched_code / total_code * 100.0) if total_code > 0 else 0.0
    matched_data_pct = (matched_data / total_data * 100.0) if total_data > 0 else (100.0 if total_data == 0 else 0.0)
    matched_fns_pct = (matched_fns / total_fns * 100.0) if total_fns > 0 else 0.0

    return {
        "fuzzy_match_percent": round(matched_code_pct, 6),
        "total_code": str(total_code),
        "matched_code": str(matched_code),
        "matched_code_percent": round(matched_code_pct, 6),
        "total_data": str(total_data),
        "matched_data": str(matched_data),
        "matched_data_percent": round(matched_data_pct, 6),
        "total_functions": int(total_fns),
        "matched_functions": int(matched_fns),
        "matched_functions_percent": round(matched_fns_pct, 6),
        "complete_code": "0",
        "complete_code_percent": 0.0,
        "complete_data": "0",
        "complete_data_percent": 0.0,
        "total_units": int(total_units),
        "complete_units": int(complete_units)
    }


# CTR-SDK nn::nex, nn::pia, nn::ssl, nn::uds, nn::frd, nn::http modular units
# Verified directly from Yo-kai Watch 2's .text / .data DDL tables, IPC headers, and debug strings
_CTR_NETWORK_UNITS = [
    # nn::nex (0x00300044 - 0x00328400)
    (0x00300500, "src/ctr/nn/nex/nex_Types.cpp"),
    (0x00301000, "src/ctr/nn/nex/nex_MemoryManager.cpp"),
    (0x00301800, "src/ctr/nn/nex/nex_RootObject.cpp"),
    (0x00302500, "src/ctr/nn/nex/nex_Object.cpp"),
    (0x00303200, "src/ctr/nn/nex/nex_String.cpp"),
    (0x00304200, "src/ctr/nn/nex/nex_Time.cpp"),
    (0x00305200, "src/ctr/nn/nex/nex_HealthService.cpp"),
    (0x00306200, "src/ctr/nn/nex/nex_CriticalSection.cpp"),
    (0x00307200, "src/ctr/nn/nex/nex_MutexPrimitive.cpp"),
    (0x00308400, "src/ctr/nn/nex/nex_ScopedCS.cpp"),
    (0x00309800, "src/ctr/nn/nex/nex_AccountManagementService.cpp"),
    (0x0030A800, "src/ctr/nn/nex/nex_MonitoringProtocol.cpp"),
    (0x0030BC00, "src/ctr/nn/nex/nex_Thread.cpp"),
    (0x0030CC00, "src/ctr/nn/nex/nex_InstanceControl.cpp"),
    (0x0030DC00, "src/ctr/nn/nex/nex_State.cpp"),
    (0x0030F000, "src/ctr/nn/nex/nex_SystemComponent.cpp"),
    (0x00310800, "src/ctr/nn/nex/nex_Exception.cpp"),
    (0x00312000, "src/ctr/nn/nex/nex_Buffer.cpp"),
    (0x00313400, "src/ctr/nn/nex/nex_MessagingService.cpp"),
    (0x00314800, "src/ctr/nn/nex/nex_ByteStream.cpp"),
    (0x00315800, "src/ctr/nn/nex/nex_BitStream.cpp"),
    (0x00316800, "src/ctr/nn/nex/nex_AccountManagementProtocol.cpp"),
    (0x00317800, "src/ctr/nn/nex/nex_DDLTypes.cpp"),
    (0x00318800, "src/ctr/nn/nex/nex_Data.cpp"),
    (0x00319800, "src/ctr/nn/nex/nex_ChecksumAlgorithm.cpp"),
    (0x0031A800, "src/ctr/nn/nex/nex_CompressionAlgorithm.cpp"),
    (0x0031B800, "src/ctr/nn/nex/nex_TicketGrantingProtocol.cpp"),
    (0x0031C800, "src/ctr/nn/nex/nex_KerberosEncryption.cpp"),
    (0x0031D800, "src/ctr/nn/nex/nex_SecureConnectionService.cpp"),
    (0x0031E800, "src/ctr/nn/nex/nex_NotificationEvent.cpp"),
    (0x0031F400, "src/ctr/nn/nex/qResult.cpp"),
    (0x00320800, "src/ctr/nn/nex/nex_CallContext.cpp"),
    (0x00322000, "src/ctr/nn/nex/nex_MatchMakingService.cpp"),
    (0x00323400, "src/ctr/nn/nex/nex_Scheduler.cpp"),
    (0x00324800, "src/ctr/nn/nex/nex_Operation.cpp"),
    (0x00325C00, "src/ctr/nn/nex/nex_NotificationProtocol.cpp"),
    (0x00327000, "src/ctr/nn/nex/nex_Core.cpp"),
    (0x00328400, "src/ctr/nn/nex/nex_NgsFacade.cpp"),
    # nn::ngword (0x00328400 - 0x00329400, IPC "ngword:")
    (0x00329400, "src/ctr/nn/ngword/ngword_Api.cpp"),
    # nn::pia::inet (0x00329400 - 0x00338800, "pia inet heap")
    (0x0032B000, "src/ctr/nn/pia/inet/pia_NatDetecter.cpp"),
    (0x0032B700, "src/ctr/nn/pia/inet/pia_NatRelay.cpp"),
    (0x0032DB00, "src/ctr/nn/pia/inet/pia_NatTraverser.cpp"),
    (0x0032E800, "src/ctr/nn/pia/inet/pia_NatDetectionJob.cpp"),
    (0x0032F200, "src/ctr/nn/pia/inet/pia_NatPortDetecter.cpp"),
    (0x0032FB00, "src/ctr/nn/pia/inet/pia_SocketOutputStream.cpp"),
    (0x00330400, "src/ctr/nn/pia/inet/pia_NatPropertyDetecter.cpp"),
    (0x00331F00, "src/ctr/nn/pia/inet/pia_NexConnectStationJob.cpp"),
    (0x00334200, "src/ctr/nn/pia/inet/pia_NexNatTraversalProtocol.cpp"),
    (0x00334600, "src/ctr/nn/pia/inet/pia_NatServerAddressResolveJob.cpp"),
    (0x00337800, "src/ctr/nn/pia/inet/pia_InetProcessHostMigrationJob.cpp"),
    (0x00338800, "src/ctr/nn/pia/inet/pia_NexFacade.cpp"),
    # nn::pia::local (0x00338800 - 0x00340300, "pia local heap")
    (0x0033B000, "src/ctr/nn/pia/local/pia_UdsNodeBase.cpp"),
    (0x0033C000, "src/ctr/nn/pia/local/pia_UdsMigrationManager.cpp"),
    (0x0033D000, "src/ctr/nn/pia/local/pia_LocalProcessHostMigrationJob.cpp"),
    (0x00340300, "src/ctr/nn/pia/local/pia_UdsNode.cpp"),
    # nn::pia::common (0x00340300 - 0x00343A00, "pia common heap")
    (0x00342600, "src/ctr/nn/pia/common/pia_NetworkFactory.cpp"),
    (0x00343A00, "src/ctr/nn/pia/common/pia_BackgroundScheduler.cpp"),
    # nn::pia::session (0x00343A00 - 0x00353980, "pia session heap")
    (0x00345500, "src/ctr/nn/pia/session/pia_JoinSessionJob.cpp"),
    (0x00346800, "src/ctr/nn/pia/session/pia_LeaveSessionJob.cpp"),
    (0x00349500, "src/ctr/nn/pia/session/pia_SessionProtocolReliable.cpp"),
    (0x00349C80, "src/ctr/nn/pia/session/pia_CreateSessionJob.cpp"),
    (0x0034A280, "src/ctr/nn/pia/session/pia_KickoutManageJob.cpp"),
    (0x0034A800, "src/ctr/nn/pia/session/pia_DestroySessionJob.cpp"),
    (0x0034B000, "src/ctr/nn/pia/session/pia_RelayRouteManageJob.cpp"),
    (0x0034BC80, "src/ctr/nn/pia/session/pia_SessionClockProtocol.cpp"),
    (0x0034D000, "src/ctr/nn/pia/session/pia_ProcessJoinRequestJob.cpp"),
    (0x0034E800, "src/ctr/nn/pia/session/pia_ProcessHostMigrationJob.cpp"),
    (0x00351700, "src/ctr/nn/pia/session/pia_ProcessUpdateSessionJob.cpp"),
    (0x00351E00, "src/ctr/nn/pia/session/pia_ProcessDestroySessionJob.cpp"),
    (0x00353980, "src/ctr/nn/pia/session/pia_Session.cpp"),
    # nn::pia::transport (0x00353980 - 0x00366000, "pia transport heap")
    (0x00355100, "src/ctr/nn/pia/transport/pia_RttProtocol.cpp"),
    (0x00356800, "src/ctr/nn/pia/transport/pia_PacketAnalyzer.cpp"),
    (0x00358C00, "src/ctr/nn/pia/transport/pia_LatencyEmulator.cpp"),
    (0x00358F80, "src/ctr/nn/pia/transport/pia_SendThreadStream.cpp"),
    (0x0035AC00, "src/ctr/nn/pia/transport/pia_ConnectStationJob.cpp"),
    (0x0035CE80, "src/ctr/nn/pia/transport/pia_RelayRouteManager.cpp"),
    (0x0035D500, "src/ctr/nn/pia/transport/pia_TransportAnalyzer.cpp"),
    (0x0035E100, "src/ctr/nn/pia/transport/pia_ConnectionAnalyzer.cpp"),
    (0x0035E880, "src/ctr/nn/pia/transport/pia_UnreliableProtocol.cpp"),
    (0x0035EF00, "src/ctr/nn/pia/transport/pia_ReceiveThreadStream.cpp"),
    (0x00361400, "src/ctr/nn/pia/transport/pia_DisconnectStationJob.cpp"),
    (0x00362700, "src/ctr/nn/pia/transport/pia_ReliableSlidingWindow.cpp"),
    (0x00363980, "src/ctr/nn/pia/transport/pia_StationProtocolManager.cpp"),
    (0x00363C00, "src/ctr/nn/pia/transport/pia_PacketParser.cpp"),
    (0x00364200, "src/ctr/nn/pia/transport/pia_StationProtocolReliable.cpp"),
    (0x00365380, "src/ctr/nn/pia/transport/pia_ProcessConnectionRequestJob.cpp"),
    (0x00366000, "src/ctr/nn/pia/transport/pia_ThreadStreamManager.cpp"),
    # nn::ssl, nn::uds, nn::frd, nn::http (0x00366000 - 0x00370000)
    (0x00367800, "src/ctr/nn/ssl/ssl_Api.cpp"),
    (0x0036A000, "src/ctr/nn/uds/uds_Api.cpp"),
    (0x0036BF00, "src/ctr/nn/frd/frd_Api.cpp"),
    (0x00370000, "src/ctr/nn/http/http_Api.cpp"),
]

_CTR_SERVICE_UNITS = [
    # Lower .text / pre-service SDK & utility units (0x0028014C - 0x002B9000)
    (0x00288000, "src/ctr/nn/fnd/fnd_ExpHeapBase.cpp"),
    (0x00290000, "src/ctr/nn/fnd/fnd_FrameHeapBase.cpp"),
    (0x00298000, "src/ctr/nn/fnd/fnd_HeapBase.cpp"),
    (0x002A0000, "src/ctr/nn/gxlow/gxlow_Gpu.cpp"),
    (0x002A8000, "src/ctr/nn/gxlow/gxlow_CmdReqQueue.cpp"),
    (0x002B0000, "src/ctr/nn/hid/hid_Api.cpp"),
    (0x002B9000, "src/ctr/nn/cfg/cfg_Api.cpp"),
    # Verified IPC service units in Yo-kai Watch 2 (0x002B9000 - 0x00300000)
    (0x002B9900, "src/ctr/nn/ac/ac_Api.cpp"),          # 0x002B915C: "ac:u"
    (0x002B9D00, "src/ctr/nn/am/am_Api.cpp"),          # 0x002B9A0C: "am:app"
    (0x002BB000, "src/ctr/nn/aoc/aoc_Api.cpp"),        # 0x002B9D90: "nim:aoc"
    (0x002BEE00, "src/ctr/nn/ec/ec_Api.cpp"),          # 0x002BC620: "sys.ECardStatus"
    (0x002C1000, "src/ctr/nn/fs/fs_Api.cpp"),          # 0x002BEF7C: fs:USER IPC 0x0801..0x0854
    (0x002C3A00, "src/ctr/nn/fs/fs_FileStream.cpp"),
    (0x002C6000, "src/ctr/nn/cec/CTR_Api.cpp"),        # 0x002C3D38: "cecd:u" IPC 0x0001..0x0012
    (0x002CA000, "src/ctr/nn/cec/cec_CecControl.cpp"),
    (0x002CF000, "src/ctr/nn/cec/cec_Message.cpp"),
    (0x002D5000, "src/ctr/nn/cec/cec_MessageBox.cpp"),
    (0x002DD000, "src/ctr/nn/cec/cec_MessageId.cpp"),
    # nn::nex DDL & protocol units before 0x00300000 + zlib 1.2.5 (0x002ED030)
    (0x002E4000, "src/ctr/nn/nex/nex_HealthProtocol.cpp"),
    (0x002EC800, "src/ctr/nn/nex/nex_MessagingProtocol.cpp"),
    (0x002F0000, "src/ctr/nn/zlib/inflate.c"),         # 0x002ED030: "1.2.5"
    (0x002F4000, "src/ctr/nn/zlib/deflate.c"),
    (0x002FA000, "src/ctr/nn/nex/nex_MatchMakingProtocol.cpp"),
    (0x00300000, "src/ctr/nn/nex/nex_Gathering.cpp"),
]

# Level-5 Engine LXP / LFW / GDS / LIZ modular classes
# Strictly named after RTTI descriptors and embedded class strings in code.raw.bin
_ENGINE_LXP_UNITS = [
    (0x0012D500, "src/engine/lxp/lxpCStack.cpp"),                # 0x0012AC84: "lxpCStack"
    (0x00131500, "src/engine/lxp/lxpCMemory.cpp"),               # 0x001310AC: "lxpCMemory"
    (0x00133600, "src/engine/lxp/lxpCObjImage.cpp"),             # 0x001333AC: "IMGC00"
    (0x00136200, "src/engine/lxp/lxpCObjMovie.cpp"),             # 0x0013368C: 12lxpCObjMovie
    # Sorted by mangled RTTI symbol name (_ZN10... to _ZN16...) in 0x00201A50 - 0x00260000
    (0x00203600, "src/engine/lxp/lxpCDrwRes.cpp"),               # 10lxpCDrwRes (0x00201A78)
    (0x00206E00, "src/engine/lxp/lxpCObjDOF.cpp"),               # 10lxpCObjDOF (0x00203654)
    (0x0020AB00, "src/engine/lfw/lfwCMessage.cpp"),              # 11lfwCMessage (0x00206F1C)
    (0x0020CB00, "src/engine/lfw/lfwCObjLine.cpp"),              # 11lfwCObjLine (0x0020AB70)
    (0x00211300, "src/engine/lxp/lxpCMessage.cpp"),              # 11lxpCMessage (0x0020CB88)
    (0x00214600, "src/engine/lxp/lxpCObjBlur.cpp"),              # 11lxpCObjBlur (0x002113DC)
    (0x00218B00, "src/engine/lfw/lfwCObject.cpp"),               # 12__lfwCObject (0x002148B8)
    (0x0021C800, "src/engine/lfw/lfwCObjImage.cpp"),             # 12lfwCObjImage (0x00218B44)
    (0x0021D280, "src/engine/lxp/lxpCObjAnime.cpp"),             # 12lxpCObjAnime (0x0021C808)
    (0x00224000, "src/engine/lxp/lxpCObjChara.cpp"),             # 12lxpCObjChara (0x0021D288)
    (0x0022F000, "src/engine/lxp/lxpCObjChara.cpp"),             # 12lxpCObjChara (0x002255A4..0x0022B690)
    (0x0022F780, "src/engine/lxp/lxpCObjColor.cpp"),             # 12lxpCObjColor (0x0022F028)
    (0x00230900, "src/engine/lxp/lxpCObjMovie.cpp"),             # 12lxpCObjMovie (0x0022F7E8)
    (0x00231B80, "src/engine/lxp/lxpCOpenGLES.cpp"),             # 0x00230960: "lxpCOpenGLES"
    (0x00233D00, "src/engine/lxp/lxpCResAnime.cpp"),             # 12lxpCResAnime (0x00231BBC)
    (0x00238800, "src/engine/lxp/lxpCResChara.cpp"),             # 12lxpCResChara (0x00233D38)
    (0x0023B380, "src/engine/lxp/lxpCResModel.cpp"),             # 12lxpCResModel (0x00238850)
    (0x0023F580, "src/engine/lxp/lxpCResource.cpp"),             # 12lxpCResource (0x0023B3BC)
    (0x00240E00, "src/engine/lxp/lxpCBGProcess.cpp"),            # 0x00240AC0: "lxpCBGProcess"
    (0x00241900, "src/engine/lxp/lxpCObjCanvas.cpp"),            # 13lxpCObjCanvas (0x00240E18)
    (0x00242200, "src/engine/lxp/lxpCObjImage2.cpp"),            # 13lxpCObjImage2 (0x00241950)
    (0x00242C80, "src/engine/lxp/lxpCObjLocus1.cpp"),            # 13lxpCObjLocus1 (0x00242284)
    (0x00249100, "src/engine/lxp/lxpCObjLocus2.cpp"),            # 13lxpCObjLocus2 (0x00242CA8)
    (0x00249400, "src/engine/lxp/lxpCObjLocus.cpp"),             # 14__lxpCObjLocus (0x0024914C)
    (0x0024DCE0, "src/engine/lxp/lxpCResourceBase.cpp"),         # 14__lxpCResource (0x00249430)
    (0x0024DE7C, "src/engine/lfw/lfwCCamera_Fix.cpp"),           # 14lfwCCamera_Fix (0x0024DD18)
    (0x0024F900, "src/engine/lfw/lfwCCamera_Mtn.cpp"),           # 14lfwCCamera_Mtn (0x0024DE7C)
    (0x0024FA40, "src/engine/lxp/lxpCObjCapture.cpp"),           # 14lxpCObjCapture (0x0024F93C)
    (0x00253C00, "src/engine/lxp/lxpCObjEasyDOF.cpp"),           # 14lxpCObjEasyDOF (0x0024FA44)
    (0x00256800, "src/engine/lfw/lfwCCamera_Move.cpp"),          # 15lfwCCamera_Move (0x00253C70)
    (0x00256B40, "src/engine/lxp/lxpCObjParticle.cpp"),          # 15lxpCObjParticle (0x002568E0)
    (0x0025B580, "src/engine/lxp/lxpCResParticle.cpp"),          # 15lxpCResParticle (0x00256B68)
    (0x0025BD80, "src/engine/lfw/lfwCCamera_Dolly.cpp"),         # 16lfwCCamera_Dolly (0x0025B5C8)
    (0x0025F680, "src/engine/lfw/lfwCCamera_Orbit.cpp"),         # 16lfwCCamera_Orbit (0x0025BDBC)
    (0x00260000, "src/engine/lxp/lxpCCameraMotion.cpp"),         # 16lxpCCameraMotion (0x0025F6C0)
]

_ENGINE_CORE_UNITS = [
    (0x0013F800, "src/engine/core/lfwCObjManager.cpp"),
    (0x00144C00, "src/engine/core/lxpCCamera.cpp"),              # 10lxpCCamera (0x0013FF98)
    (0x0014DF00, "src/engine/core/lxpCResource.cpp"),            # 0x00144CBC: "RES.bin"
    (0x00153D00, "src/engine/core/lxpCResParticle.cpp"),         # 15lxpCResParticle (0x0014DF3C)
    (0x00154A00, "src/engine/core/lxpCMemObj.cpp"),              # 10lxpCMemObj (0x00153DF0)
    (0x00159600, "src/engine/core/lxpCResChara.cpp"),            # 12lxpCResChara (0x00154A20)
    (0x0015E3E0, "src/engine/core/lxpCResourceBase.cpp"),        # 12lxpCResource (0x00159628)
    (0x00168500, "src/engine/core/lxpCLoop.cpp"),                # 8lxpCLoop (0x0015E404)
    (0x0016E000, "src/engine/core/lfwCObject.cpp"),              # 12__lfwCObject (0x001685C4)
    (0x00172300, "src/engine/core/lfwCObj3D.cpp"),               # 9lfwCObj3D (0x0016E084)
    (0x00176000, "src/engine/core/lxpCObjDOF.cpp"),              # 10lxpCObjDOF (0x00172314)
    (0x00180000, "src/engine/core/lxpCSkyManager.cpp"),
    (0x0018A000, "src/engine/core/lxpCFontManager.cpp"),
    (0x00194000, "src/engine/gds/GDSFix.cpp"),                   # "GDSFix"
    (0x0019E000, "src/engine/gds/GDSSwitch.cpp"),                # "GDSSwitch"
    (0x001A8000, "src/engine/core/lxpCAnimeManager.cpp"),
    (0x001B2000, "src/engine/core/lxpCCharaManager.cpp"),
    (0x001BC000, "src/engine/core/lxpCImageManager.cpp"),
    (0x001C6000, "src/engine/core/lxpCLightManager.cpp"),
    (0x001D0000, "src/engine/core/lxpCModelManager.cpp"),
    (0x001DA000, "src/engine/core/lxpCMovieManager.cpp"),
    (0x001E4000, "src/engine/core/lxpCParticleManager.cpp"),
    (0x001EE000, "src/engine/core/lxpCResourceManagerBase.cpp"),
    (0x001F8000, "src/engine/core/lxpCResourceManagerRef.cpp"),
    (0x00201A50, "src/engine/core/lxpCMessage.cpp"),             # 11lxpCMessage (0x001FDB58)
    # 0x00260000 - 0x0028014C (_ZN16.._ZN18 + BG managers)
    (0x00264380, "src/engine/core/lxpCCameraMotion.cpp"),        # 16lxpCCameraMotion (0x002601C8)
    (0x00267900, "src/engine/core/lxpCBGLoadManager.cpp"),       # 0x00265D60: "lxpCBGLoadManager"
    (0x0026AE00, "src/engine/core/lxpCCameraControl.cpp"),       # 17lxpCCameraControl (0x00267950)
    (0x00273B00, "src/engine/core/lxpCObjCharaMerge.cpp"),       # 17lxpCObjCharaMerge (0x0026AE24)
    (0x00274D00, "src/engine/core/lxpCBGBuildManager.cpp"),      # 0x00273B60: "lxpCBGBuildManager"
    (0x00280200, "src/engine/core/lxpCObjSimpleModel.cpp"),      # 18lxpCObjSimpleModel (0x00274D04)
    # 0x00580000 - 0x006F32AC (_ZN3liz..., _ZN5CLcs..., _ZN8lfw..., _ZN9lfw..., _ZNK...)
    (0x00590000, "src/engine/core/lfwCCamera_Dev.cpp"),
    (0x005A0000, "src/engine/core/lfwCCamera_Fix.cpp"),
    (0x005B0000, "src/engine/core/lfwCCamera_Mtn.cpp"),
    (0x005C0000, "src/engine/core/lfwCCamera_Dolly.cpp"),
    (0x005D3000, "src/engine/core/lfwCCamera_Orbit.cpp"),
    (0x005DF000, "src/engine/gds/GDSSwitch.cpp"),                # 0x005D3208..0x005DE020: "GDSFix" / "GDSSwitch"
    (0x005E5500, "src/engine/gds/GDSFix.cpp"),
    (0x005E78E0, "src/engine/liz/ResBaseLXP.cpp"),               # N3liz10ResBaseLXPE (0x005E5588)
    (0x005E7B40, "src/engine/liz/ResAnimeLXP.cpp"),              # N3liz11ResAnimeLXPE (0x005E78F8)
    (0x005E7DA0, "src/engine/liz/ResCharaLXP.cpp"),              # N3liz11ResCharaLXPE (0x005E7B58)
    (0x005E8020, "src/engine/liz/ResImageLXP.cpp"),              # N3liz11ResImageLXPE (0x005E7DC4)
    (0x005E8200, "src/engine/liz/ResModelLXP.cpp"),              # N3liz11ResModelLXPE (0x005E803C)
    (0x005EBB50, "src/engine/liz/ResObj2DLXP.cpp"),              # N3liz11ResObj2DLXPE (0x005E8228)
    (0x005EBD00, "src/engine/liz/IResource.cpp"),                # N3liz9IResourceE (0x005EBB60)
    (0x005EF000, "src/engine/liz/ResSkyLXP.cpp"),                # N3liz9ResSkyLXPE (0x005EBD20)
    (0x005F0800, "src/engine/core/CLcs1.cpp"),                   # 5CLcs1 (0x005F0648)
    (0x005F8000, "src/engine/core/CLcs2.cpp"),                   # 5CLcs2 (0x005F087C)
    (0x00608000, "src/engine/core/lfwCObj2DManager.cpp"),
    (0x0061AA00, "src/engine/core/lfwCObj3DManager.cpp"),
    (0x0061DC40, "src/engine/lfw/lfwCLoop.cpp"),                 # 8lfwCLoop (0x0061AAE0)
    (0x00621800, "src/engine/core/lxpCLoop.cpp"),                # 8lxpCLoop (0x0061DC50)
    (0x00622660, "src/engine/core/lfwCObj2D.cpp"),               # 9lfwCObj2D (0x00621808)
    (0x00625000, "src/engine/core/lfwCObj3D.cpp"),               # 9lfwCObj3D (0x00622674)
    # _ZNK... (const methods in alphabetical mangled order, 0x00625008 - 0x00677000)
    (0x00625B00, "src/engine/lxp/lxpCDrwRes.cpp"),               # _ZNK10lxpCDrwRes (0x00625008)
    (0x00626960, "src/engine/lfw/lfwCMessage.cpp"),              # _ZNK11lfwCMessage (0x00625B58)
    (0x00626C80, "src/engine/lfw/lfwCObjLine.cpp"),              # _ZNK11lfwCObjLine (0x00626978)
    (0x00627260, "src/engine/lxp/lxpCMessage.cpp"),              # _ZNK11lxpCMessage (0x00626C84)
    (0x006283C0, "src/engine/lxp/lxpCObjBlur.cpp"),              # _ZNK11lxpCObjBlur (0x00627278)
    (0x00628EA0, "src/engine/lfw/lfwCObjImage.cpp"),             # _ZNK12lfwCObjImage (0x006283D4)
    (0x006290E0, "src/engine/lxp/lxpCObjAnime.cpp"),             # _ZNK12lxpCObjAnime (0x00628EBC)
    (0x0062AC40, "src/engine/lxp/lxpCObjChara.cpp"),             # _ZNK12lxpCObjChara (0x006290F0)
    (0x0062B600, "src/engine/lxp/lxpCResAnime.cpp"),             # _ZNK12lxpCResAnime (0x0062AF98)
    (0x0062D2E0, "src/engine/lxp/lxpCResChara.cpp"),             # _ZNK12lxpCResChara (0x0062B614)
    (0x0062E560, "src/engine/lxp/lxpCResModel.cpp"),             # _ZNK12lxpCResModel (0x0062D2E8)
    (0x0062F080, "src/engine/lxp/lxpCObjLocus1.cpp"),            # _ZNK13lxpCObjLocus1 (0x0062E588)
    (0x00630200, "src/engine/lxp/lxpCObjCapture.cpp"),           # _ZNK14lxpCObjCapture (0x0062F644)
    (0x00635000, "src/engine/lxp/lxpCResParticle.cpp"),          # _ZNK15lxpCResParticle (0x00630714)
    (0x00642000, "src/engine/core/lxpCObjCharaMerge.cpp"),       # _ZNK17lxpCObjCharaMerge (0x00635154)
    (0x00652000, "src/engine/core/lfwCCameraManager.cpp"),
    (0x00662000, "src/engine/core/lxpCCameraControl.cpp"),
    (0x00673A80, "src/engine/core/lxpCObjSimpleModel.cpp"),
    (0x00674590, "src/engine/lfw/lfwCLoop.cpp"),                 # _ZNK8lfwCLoop (0x00673A8C)
    (0x006753A0, "src/engine/core/lxpCLoop.cpp"),                # _ZNK8lxpCLoop (0x006745A0)
    (0x00676800, "src/engine/core/lfwCObj3D.cpp"),               # _ZNK9lfwCObj3D (0x006753AC)
    (0x00684000, "src/engine/core/lxpCObjParticle.cpp"),         # 0x0067684C: 15lxpCObjParticle
    (0x00694000, "src/engine/core/lxpCMemory.cpp"),              # 0x0067A3A8: "lxp_mem_sys" / "lxp_mem_dev"
    (0x006A4000, "src/engine/core/lxpCStack.cpp"),
    (0x006B4000, "src/engine/core/lxpCObjEasyDOF.cpp"),          # 14lxpCObjEasyDOF (0x006B3034)
    (0x006C4000, "src/engine/core/lxpCCameraMotion.cpp"),        # 16lxpCCameraMotion (0x006B2410)
    (0x006D4000, "src/engine/core/lxpCOpenGLES.cpp"),
    (0x006E4000, "src/engine/core/lxpCBGProcess.cpp"),
    (0x00700000, "src/engine/core/lfwCLoop.cpp"),                # 8lfwCLoop (0x006EDA5C)
]


def _lookup_range_unit(vma: int, table: list, default_unit: str) -> str:
    for upper_vma, unit_path in table:
        if vma < upper_vma:
            return unit_path
    return default_unit


def assign_target_unit(fn: dict) -> str:
    """Pre-groups functions into canonical target C/C++ compilation units (.cpp / .c)."""
    sym = fn["symbol"]
    layer = fn["layer"]
    subsys = fn["subsystem"]
    vma = fn.get("address", 0)
    dem = demangle(sym)

    # 1. Game Layer: keep untouched as requested ("excepto, el juego, el juego dejalo asi")
    if subsys == "loops" or layer == "game":
        m = re.match(r"^(ywCLoop[A-Za-z0-9]+)::", dem)
        if m:
            return f"src/game/loops/{m.group(1)}.cpp"
        if subsys == "loops":
            return "src/game/loops/ywCLoop.cpp"
        return f"src/{layer}/{subsys}/{subsys}.cpp"

    # 2. CTR: nn::nex, nn::pia, nn::ssl, nn::uds, nn::frd, nn::http
    if subsys == "nn_nex":
        m = re.match(r"^nn::nex::([A-Za-z0-9]+)::", dem)
        if m:
            return f"src/ctr/nn/nex/{m.group(1)}.cpp"
        if vma == 0x00301248:
            return "src/ctr/nn/nex/nex_RootObject.cpp"
        return _lookup_range_unit(vma, _CTR_NETWORK_UNITS, "src/ctr/nn/http/http_Api.cpp")

    # 3. CTR: standard Nintendo CTR-SDK subsystems
    if subsys == "nn_applet":
        return "src/ctr/nn/applet/applet.cpp"

    if subsys == "nn_dsp":
        dsp_driver_vmas = {
            0x001384E8, 0x001385EC, 0x0013862C, 0x00138660, 0x00138688,
            0x001387E4, 0x00138824, 0x00138858, 0x00138880, 0x001388EC,
            0x001389F4, 0x00138A30, 0x00138A58, 0x00138AB8, 0x00138B64,
            0x00138C30
        }
        if vma in dsp_driver_vmas:
            return "src/ctr/nn/dsp/dsp_DSP.cpp"
        elif 0x00138E24 <= vma <= 0x00138FEC:
            return "src/ctr/nn/dsp/dsp_IirFilter.cpp"
        return "src/ctr/nn/dsp/CTR_Api.cpp"

    if subsys == "nn_snd":
        return "src/ctr/nn/snd/snd_Dspsnd.cpp"

    if subsys == "nn_ndm":
        return "src/ctr/nn/ndm/ndm_Api.cpp"

    if subsys == "nn_y2r":
        if vma < 0x00113700:
            return "src/ctr/nn/y2r/y2r_Y2r.cpp"
        return "src/ctr/nn/y2r/detail_Api.cpp"

    if subsys == "nn_math":
        if vma <= 0x00151A00:
            return "src/ctr/nn/math/math_ARMv6Asm.cpp"
        return "src/ctr/nn/math/math_MTX44.cpp"

    if subsys == "nn_os":
        os_api_vmas = {
            0x00104D18, 0x001063F4, 0x00106504, 0x00106518, 0x0010655C,
            0x0010678C, 0x00127520, 0x001275D8, 0x0012788C
        }
        if vma in os_api_vmas or vma < 0x00106800:
            return "src/ctr/nn/os/os_Api.cpp"
        elif vma < 0x00112EB0:
            return "src/ctr/nn/os/os_Thread.cpp"
        elif vma < 0x00113200:
            return "src/ctr/nn/os/os_MemoryBlock.cpp"
        elif vma < 0x00127500:
            return "src/ctr/nn/os/os_CriticalSection.cpp"
        elif vma < 0x00127800:
            return "src/ctr/nn/os/MPCore_Api.cpp"
        elif vma < 0x00127C00:
            return "src/ctr/nn/os/os_EventBase.cpp"
        elif vma < 0x00128000:
            return "src/ctr/nn/os/os_LightEvent.cpp"
        return "src/ctr/nn/os/os_WaitObject.cpp"

    if subsys == "nn_cecd":
        return _lookup_range_unit(vma, _CTR_SERVICE_UNITS, "src/ctr/nn/cec/cec_CecControl.cpp")

    if subsys == "svc":
        return "src/ctr/svc/svc_Api.c"

    if subsys == "crt":
        if vma < 0x00100200:
            return "src/ctr/crt/crt0.c"
        elif vma < 0x00101400:
            return "src/ctr/crt/aeabi_memcpy.c"
        elif vma < 0x00102600:
            return "src/ctr/crt/aeabi_arith.c"
        elif vma < 0x00103800:
            return "src/ctr/crt/cpp_runtime.cpp"
        return "src/ctr/crt/cpp_exception.cpp"

    # 4. Engine subsystems (Level-5 LFW / LXP / GDS / LIZ modular classes)
    if layer == "engine":
        if subsys == "lfw":
            if vma in (0x0010B6D0, 0x0010B6D8, 0x0010B6E4):
                return "src/engine/lfw/lfwCLoopEmpty.cpp"
            elif vma in (0x0010B888, 0x0010BBB8, 0x0010C598, 0x0010C5C8, 0x0010C6A4) or vma < 0x0010C800:
                return "src/engine/lfw/lfwCLoop.cpp"
            elif vma < 0x00117000:
                return "src/engine/lfw/lxpCCameraControl.cpp"
            elif vma < 0x0011E000:
                return "src/engine/lfw/lxpCResourceManagerBase.cpp"
            elif vma < 0x00121500:
                return "src/engine/lfw/lfwCObject.cpp"
            elif vma < 0x00123000:
                return "src/engine/lfw/lxpCFontManager.cpp"
            elif vma < 0x00124800:
                return "src/engine/lfw/lfwCObj2DManager.cpp"
            elif vma < 0x00125300:
                return "src/engine/lfw/lfwCObj3DManager.cpp"
            elif vma < 0x00125C00:
                return "src/engine/lfw/lxpCModelManager.cpp"
            return "src/engine/lfw/lfwCCameraManager.cpp"

        if subsys == "lxp":
            if vma in (0x0012CB00, 0x0012FEC0, 0x00130018, 0x001300B0):
                return "src/engine/lxp/lxpCMemory.cpp"
            return _lookup_range_unit(vma, _ENGINE_LXP_UNITS, "src/engine/lxp/lxpCMemory.cpp")

        if vma == 0x0013F29C:
            return "src/engine/core/lfwCObjManager.cpp"
        return _lookup_range_unit(vma, _ENGINE_CORE_UNITS, "src/engine/core/lfwCLoop.cpp")

    # 5. Default fallback
    return f"src/{layer}/{subsys}/{subsys}.cpp"


def main():
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    all_funcs = load_all_functions()

    # Track matched status
    matched_statuses = ("matched", "community_matched", "matched_community")

    # Pre-group all functions by their target translation unit (.cpp / .c)
    units_map = {}
    for fn in all_funcs:
        unit_path = assign_target_unit(fn)
        if unit_path not in units_map:
            units_map[unit_path] = []
        units_map[unit_path].append(fn)

    # Calculate global metrics
    matched_funcs_list = [f for f in all_funcs if f["status"] in matched_statuses]
    total_code_matched = sum(f["size"] for f in matched_funcs_list)
    total_matched_functions_count = len(matched_funcs_list)

    # Layer breakdowns
    layer_stats = {
        "ctr": {"code": 0, "matched_code": 0, "fns": 0, "matched_fns": 0, "units": 0, "complete_units": 0},
        "engine": {"code": 0, "matched_code": 0, "fns": 0, "matched_fns": 0, "units": 0, "complete_units": 0},
        "game": {"code": 0, "matched_code": 0, "fns": 0, "matched_fns": 0, "units": 0, "complete_units": 0},
    }

    report_units = []

    # 1. Unit: extracted_data (.rodata + .data + .bss)
    data_unit = {
        "name": "extracted_data",
        "measures": {
            "fuzzy_match_percent": 0.0,
            "total_code": "0",
            "matched_code": "0",
            "matched_code_percent": 0.0,
            "total_data": str(TOTAL_ALL_DATA_BYTES),
            "matched_data": "0",
            "matched_data_percent": 0.0,
            "total_functions": 0,
            "matched_functions": 0,
            "matched_functions_percent": 0.0,
            "complete_code": "0",
            "complete_code_percent": 0.0,
            "complete_data": "0",
            "complete_data_percent": 0.0,
            "total_units": 1,
            "complete_units": 0
        },
        "sections": [
            {"name": ".rodata", "size": str(TOTAL_RODATA_BYTES), "fuzzy_match_percent": 0.0},
            {"name": ".data", "size": str(TOTAL_DATA_BYTES), "fuzzy_match_percent": 0.0},
            {"name": ".bss", "size": str(TOTAL_BSS_BYTES), "fuzzy_match_percent": 0.0}
        ],
        "functions": [],
        "metadata": {
            "progress_categories": ["codebin"],
            "auto_generated": False
        }
    }
    report_units.append(data_unit)

    # Calculate total code accounted across grouped units
    total_grouped_code = sum(sum(f["size"] for f in fns) for fns in units_map.values())
    unmapped_code_slack = max(0, TOTAL_CODE_BYTES - total_grouped_code)

    # 2. Add each pre-grouped C++ / C compilation unit
    for unit_path, fns in sorted(units_map.items()):
        unit_code = sum(f["size"] for f in fns)
        unit_matched_code = sum(f["size"] for f in fns if f["status"] in matched_statuses)
        unit_fns_count = len(fns)
        unit_matched_fns = len([f for f in fns if f["status"] in matched_statuses])

        # Absorb unmapped binary remainder into game/core/core.cpp so total_code reaches exact 6,238,892 B
        if unit_path == "src/game/core/core.cpp" and unmapped_code_slack > 0:
            unit_code += unmapped_code_slack

        is_complete = (unit_matched_code == unit_code and unit_code > 0)

        layer = fns[0]["layer"] if fns else "game"
        if layer not in layer_stats:
            layer = "game"

        layer_stats[layer]["code"] += unit_code
        layer_stats[layer]["matched_code"] += unit_matched_code
        layer_stats[layer]["fns"] += unit_fns_count
        layer_stats[layer]["matched_fns"] += unit_matched_fns
        layer_stats[layer]["units"] += 1
        if is_complete:
            layer_stats[layer]["complete_units"] += 1

        fn_entries = []
        for fn in fns:
            vma = fn["address"]
            addr_rel = vma - BASE_VMA
            is_m = fn["status"] in matched_statuses
            pct = 100.0 if is_m else 0.0

            fn_entries.append({
                "name": fn["symbol"],
                "size": str(fn["size"]),
                "fuzzy_match_percent": pct,
                "metadata": {
                    "demangled_name": demangle(fn["symbol"]),
                    "virtual_address": str(vma)
                },
                "address": str(addr_rel)
            })

        unit_exists_on_disk = (PROJECT_ROOT / unit_path).exists()
        unit_obj = {
            "name": unit_path,
            "measures": build_measures_dict(
                total_code=unit_code,
                matched_code=unit_matched_code,
                total_data=0,
                matched_data=0,
                total_fns=unit_fns_count,
                matched_fns=unit_matched_fns,
                total_units=1,
                complete_units=1 if is_complete else 0
            ),
            "sections": [
                {
                    "name": ".text",
                    "size": str(unit_code),
                    "fuzzy_match_percent": round((unit_matched_code / unit_code * 100.0), 6) if unit_code > 0 else 0.0
                }
            ],
            "functions": fn_entries,
            "metadata": {
                "progress_categories": [layer, "codebin"],
                "auto_generated": not unit_exists_on_disk,
                "source_path": unit_path
            }
        }
        report_units.append(unit_obj)

    total_units_count = len(report_units)
    complete_units_count = sum(1 for u in report_units if u["measures"]["complete_units"] > 0)

    # Top-level measures
    global_measures = build_measures_dict(
        total_code=TOTAL_CODE_BYTES,
        matched_code=total_code_matched,
        total_data=TOTAL_ALL_DATA_BYTES,
        matched_data=0,
        total_fns=TOTAL_FUNCTIONS_EXPECTED,
        matched_fns=total_matched_functions_count,
        total_units=total_units_count,
        complete_units=complete_units_count
    )

    # Categories array (version 2)
    categories = [
        {
            "id": "codebin",
            "name": "code.bin (all)",
            "measures": global_measures
        },
        {
            "id": "ctr",
            "name": "Nintendo CTR-SDK (ctr)",
            "measures": build_measures_dict(
                total_code=layer_stats["ctr"]["code"],
                matched_code=layer_stats["ctr"]["matched_code"],
                total_data=0,
                matched_data=0,
                total_fns=layer_stats["ctr"]["fns"],
                matched_fns=layer_stats["ctr"]["matched_fns"],
                total_units=layer_stats["ctr"]["units"],
                complete_units=layer_stats["ctr"]["complete_units"]
            )
        },
        {
            "id": "engine",
            "name": "Level-5 Engine (lfw / lxp / gds)",
            "measures": build_measures_dict(
                total_code=layer_stats["engine"]["code"],
                matched_code=layer_stats["engine"]["matched_code"],
                total_data=0,
                matched_data=0,
                total_fns=layer_stats["engine"]["fns"],
                matched_fns=layer_stats["engine"]["matched_fns"],
                total_units=layer_stats["engine"]["units"],
                complete_units=layer_stats["engine"]["complete_units"]
            )
        },
        {
            "id": "game",
            "name": "Yo-kai Watch 2 (Game Logic)",
            "measures": build_measures_dict(
                total_code=layer_stats["game"]["code"],
                matched_code=layer_stats["game"]["matched_code"],
                total_data=0,
                matched_data=0,
                total_fns=layer_stats["game"]["fns"],
                matched_fns=layer_stats["game"]["matched_fns"],
                total_units=layer_stats["game"]["units"],
                complete_units=layer_stats["game"]["complete_units"]
            )
        }
    ]

    report = {
        "version": 2,
        "measures": global_measures,
        "categories": categories,
        "units": report_units
    }

    with open(REPORT_OUTPUT, "w", encoding="utf-8") as f:
        json.dump(report, f, separators=(',', ':'))

    print(f"✅ build/report.json generated successfully in objdiff-cli v2 format ({len(report_units)} units, {total_matched_functions_count}/{TOTAL_FUNCTIONS_EXPECTED} functions matched).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
