#!/usr/bin/env python3
"""
tools/generate_report.py — Generates a comprehensive decompilation progress report
in the standard objdiff-cli format (version 2), perfectly compatible with decomp.dev,
frogress, and matching the exact schema used in reference projects (like Animal Crossing).
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


# ACNL-style CTR-SDK and NEX modular translation units
_ACNL_NEX_UNITS = [
    (0x00300500, "src/ctr/nn/nex/nex_Types.cpp"),
    (0x00301000, "src/ctr/nn/nex/nex_MemoryManager.cpp"),
    (0x00301800, "src/ctr/nn/nex/nex_RootObject.cpp"),
    (0x00302500, "src/ctr/nn/nex/nex_Object.cpp"),
    (0x00303200, "src/ctr/nn/nex/nex_String.cpp"),
    (0x00304200, "src/ctr/nn/nex/nex_Time.cpp"),
    (0x00305000, "src/ctr/nn/nex/nex_Period.cpp"),
    (0x00306000, "src/ctr/nn/nex/nex_CriticalSection.cpp"),
    (0x00306E00, "src/ctr/nn/nex/nex_MutexPrimitive.cpp"),
    (0x00307C00, "src/ctr/nn/nex/nex_ScopedCS.cpp"),
    (0x00308A00, "src/ctr/nn/nex/nex_Lock.cpp"),
    (0x00309800, "src/ctr/nn/nex/nex_Thread.cpp"),
    (0x0030A800, "src/ctr/nn/nex/nex_ObjectThread.cpp"),
    (0x0030B800, "src/ctr/nn/nex/nex_GlobalVariables.cpp"),
    (0x0030C800, "src/ctr/nn/nex/nex_InstanceControl.cpp"),
    (0x0030D800, "src/ctr/nn/nex/nex_State.cpp"),
    (0x0030E800, "src/ctr/nn/nex/nex_SystemComponent.cpp"),
    (0x0030F800, "src/ctr/nn/nex/nex_SystemChecker.cpp"),
    (0x00310800, "src/ctr/nn/nex/nex_Exception.cpp"),
    (0x00311800, "src/ctr/nn/nex/nex_Watermark.cpp"),
    (0x00312800, "src/ctr/nn/nex/nex_Plugin.cpp"),
    (0x00313800, "src/ctr/nn/nex/nex_Buffer.cpp"),
    (0x00314800, "src/ctr/nn/nex/nex_ByteStream.cpp"),
    (0x00315800, "src/ctr/nn/nex/nex_BitStream.cpp"),
    (0x00316800, "src/ctr/nn/nex/nex_Variant.cpp"),
    (0x00317800, "src/ctr/nn/nex/nex_DDLTypes.cpp"),
    (0x00318800, "src/ctr/nn/nex/nex_Data.cpp"),
    (0x00319800, "src/ctr/nn/nex/nex_ChecksumAlgorithm.cpp"),
    (0x0031A800, "src/ctr/nn/nex/nex_CompressionAlgorithm.cpp"),
    (0x0031B800, "src/ctr/nn/nex/nex_SignatureGenerator.cpp"),
    (0x0031C800, "src/ctr/nn/nex/nex_KerberosEncryption.cpp"),
    (0x0031D800, "src/ctr/nn/nex/nex_Event.cpp"),
    (0x0031E800, "src/ctr/nn/nex/nex_NotificationEvent.cpp"),
    (0x0031F400, "src/ctr/nn/nex/qResult.cpp"),
    (0x00320400, "src/ctr/nn/nex/nex_CallContext.cpp"),
    (0x00321400, "src/ctr/nn/nex/nex_Job.cpp"),
    (0x00322400, "src/ctr/nn/nex/nex_StepSequenceJob.cpp"),
    (0x00323400, "src/ctr/nn/nex/nex_Scheduler.cpp"),
    (0x00324400, "src/ctr/nn/nex/nex_Operation.cpp"),
    (0x00325400, "src/ctr/nn/nex/nex_OperationManager.cpp"),
    (0x00326400, "src/ctr/nn/nex/nex_Core.cpp"),
    (0x00327400, "src/ctr/nn/nex/nex_Api.cpp"),
    (0x00328400, "src/ctr/nn/nex/nex_NgsFacade.cpp"),
    (0x00329200, "src/ctr/nn/ngc/ngc_ProfanityFilter.cpp"),
    (0x0032A000, "src/ctr/nn/nex/nex_NatDetecter.cpp"),
    (0x0032B000, "src/ctr/nn/nex/nex_NatRelay.cpp"),
    (0x0032DD00, "src/ctr/nn/nex/nex_NatTraverser.cpp"),
    (0x0032EC00, "src/ctr/nn/nex/nex_NatPortDetecter.cpp"),
    (0x0032F800, "src/ctr/nn/nex/nex_SocketDriver.cpp"),
    (0x00330300, "src/ctr/nn/nex/nex_NatPropertyDetecter.cpp"),
    (0x00331A00, "src/ctr/nn/nex/nex_ConnectStationJob.cpp"),
    (0x00334000, "src/ctr/nn/nex/nex_NatTraversalClient.cpp"),
    (0x00334900, "src/ctr/nn/nex/nex_NatServerAddressResolveJob.cpp"),
    (0x00336800, "src/ctr/nn/nex/nex_HostMigrationJob.cpp"),
    (0x00337800, "src/ctr/nn/nex/nex_BerkeleySocketDriver.cpp"),
    (0x00338C00, "src/ctr/nn/nex/nex_Network.cpp"),
    (0x0033A800, "src/ctr/nn/uds/uds_NodeBase.cpp"),
    (0x0033C000, "src/ctr/nn/uds/uds_MigrationManager.cpp"),
    (0x00340000, "src/ctr/nn/uds/uds_Node.cpp"),
    (0x00340D50, "src/ctr/nn/nex/NexElement.cpp"),
    (0x00341200, "src/ctr/nn/nex/StationConnectionInfo.cpp"),
    (0x00342800, "src/ctr/nn/nex/nex_StationUrl.cpp"),
    (0x00343A00, "src/ctr/nn/nex/nex_Station.cpp"),
    (0x00345500, "src/ctr/nn/nex/nex_JoinSessionJob.cpp"),
    (0x00346800, "src/ctr/nn/nex/nex_LeaveSessionJob.cpp"),
    (0x00348000, "src/ctr/nn/nex/nex_Protocol.cpp"),
    (0x00349400, "src/ctr/nn/nex/nex_ClientProtocol.cpp"),
    (0x00349C00, "src/ctr/nn/nex/nex_CreateSessionJob.cpp"),
    (0x0034A800, "src/ctr/nn/nex/nex_DestroySessionJob.cpp"),
    (0x0034B000, "src/ctr/nn/nex/nex_RoutingTable.cpp"),
    (0x0034C200, "src/ctr/nn/nex/nex_SessionClockProtocol.cpp"),
    (0x0034E000, "src/ctr/nn/nex/nex_MatchmakeSession.cpp"),
    (0x00351500, "src/ctr/nn/nex/nex_MatchmakeExtensionClient.cpp"),
    (0x00353800, "src/ctr/nn/nex/nex_Session.cpp"),
    (0x00355000, "src/ctr/nn/nex/nex_RMCContext.cpp"),
    (0x00356800, "src/ctr/nn/nex/nex_Packet.cpp"),
    (0x00357800, "src/ctr/nn/nex/nex_PacketEncoder.cpp"),
    (0x00358C00, "src/ctr/nn/nex/nex_PacketDispatched.cpp"),
    (0x0035A000, "src/ctr/nn/nex/nex_ConnectionManager.cpp"),
    (0x0035CE00, "src/ctr/nn/nex/nex_PRUDPEndPoint.cpp"),
    (0x0035E000, "src/ctr/nn/nex/nex_EndPoint.cpp"),
    (0x0035EF00, "src/ctr/nn/nex/nex_Stream.cpp"),
    (0x00361000, "src/ctr/nn/nex/nex_StationProbe.cpp"),
    (0x00362600, "src/ctr/nn/nex/nex_SlidingWindow.cpp"),
    (0x00364000, "src/ctr/nn/nex/nex_Message.cpp"),
    (0x00365200, "src/ctr/nn/nex/nex_StreamManager.cpp"),
    (0x00366400, "src/ctr/nn/nex/nex_Transport.cpp"),
    (0x00367800, "src/ctr/nn/ssl/ssl_Api.cpp"),
    (0x00369000, "src/ctr/nn/nex/nex_Credentials.cpp"),
    (0x0036A400, "src/ctr/nn/nex/nex_User.cpp"),
    (0x0036B800, "src/ctr/nn/nex/nex_ServiceClient.cpp"),
    (0x0036CC00, "src/ctr/nn/nex/nex_HttpConnection.cpp"),
    (0x0036DC00, "src/ctr/nn/nex/nex_AuthenticationClient.cpp"),
    (0x0036EC00, "src/ctr/nn/nex/nex_SecureConnectionClient.cpp"),
    (0x0036F400, "src/ctr/nn/nex/nex_DataStoreClient.cpp"),
    (0x00370000, "src/ctr/nn/nex/nex_BackEndServices.cpp"),
]

_ACNL_CTR_SERVICE_UNITS = [
    (0x00283000, "src/ctr/nn/fs/fs_Api.cpp"),
    (0x00286000, "src/ctr/nn/fs/fs_FileStream.cpp"),
    (0x00289000, "src/ctr/nn/fs/fs_UserFileSystem.cpp"),
    (0x0028C000, "src/ctr/nn/fs/fs_Archive.cpp"),
    (0x0028F000, "src/ctr/nn/fnd/fnd_ExpHeapBase.cpp"),
    (0x00292000, "src/ctr/nn/fnd/fnd_FrameHeapBase.cpp"),
    (0x00295000, "src/ctr/nn/fnd/fnd_TimeSpan.cpp"),
    (0x00298000, "src/ctr/nn/gxlow/gxlow_Gpu.cpp"),
    (0x0029B000, "src/ctr/nn/gxlow/gxlow_CmdReqQueue.cpp"),
    (0x0029E000, "src/ctr/nn/hid/hid_Api.cpp"),
    (0x002A1000, "src/ctr/nn/hid/hid_PadReader.cpp"),
    (0x002A4000, "src/ctr/nn/hid/hid_TouchPanelReader.cpp"),
    (0x002A7000, "src/ctr/nn/hid/hid_AccelerometerReader.cpp"),
    (0x002AA000, "src/ctr/nn/hid/hid_GyroscopeReader.cpp"),
    (0x002AD000, "src/ctr/nn/cfg/CTR_Api.cpp"),
    (0x002B0000, "src/ctr/nn/srv/srv_Api.cpp"),
    (0x002B3000, "src/ctr/nn/err/err_FatalErr.cpp"),
    (0x002B6000, "src/ctr/nn/ptm/ptm_Api.cpp"),
    (0x002B9500, "src/ctr/nn/ac/ac_Api.cpp"),
    (0x002B9C00, "src/ctr/nn/am/am_Api.cpp"),
    (0x002BC000, "src/ctr/nn/nim/nim_Api.cpp"),
    (0x002BFA00, "src/ctr/nn/ec/ec_Api.cpp"),
    (0x002C3000, "src/ctr/nn/ec/ec_Shopping.cpp"),
    (0x002C6000, "src/ctr/nn/cec/CTR_Api.cpp"),
    (0x002C9000, "src/ctr/nn/cec/cec_CecControl.cpp"),
    (0x002CC000, "src/ctr/nn/cec/cec_CecControlSys.cpp"),
    (0x002CF000, "src/ctr/nn/cec/cec_Message.cpp"),
    (0x002D2000, "src/ctr/nn/cec/cec_MessageBox.cpp"),
    (0x002D5000, "src/ctr/nn/cec/cec_MessageId.cpp"),
    (0x002D8000, "src/ctr/nn/boss/boss_Api.cpp"),
    (0x002DB000, "src/ctr/nn/boss/boss_Task.cpp"),
    (0x002DE000, "src/ctr/nn/frd/frd_Api.cpp"),
    (0x002E1000, "src/ctr/nn/act/act_Api.cpp"),
    (0x002E4000, "src/ctr/nn/ir/ir_Api.cpp"),
    (0x002E7000, "src/ctr/nn/ro/ro_Api.cpp"),
    (0x002EA000, "src/ctr/nn/crypto/crypto_AesImpl.cpp"),
    (0x002EC800, "src/ctr/nn/crypto/crypto_Sha256Context.cpp"),
    (0x002EF800, "src/ctr/nn/zlib/inflate.c"),
    (0x002F2800, "src/ctr/nn/zlib/deflate.c"),
    (0x002F6000, "src/ctr/nn/crypto/crypto_RsaContext.cpp"),
    (0x002FA000, "src/ctr/nn/soc/soc_Api.cpp"),
    (0x002FD000, "src/ctr/nn/http/http_Api.cpp"),
    (0x00300000, "src/ctr/nn/uds/CTR_Api.cpp"),
]

_ENGINE_LXP_UNITS = [
    (0x0012D500, "src/engine/lxp/lxpCStack.cpp"),
    (0x00131500, "src/engine/lxp/lxpCMemory.cpp"),
    (0x00134000, "src/engine/lxp/lxpCImageLoader.cpp"),
    (0x00136200, "src/engine/lxp/lxpCSoundStream.cpp"),
    (0x00202000, "src/engine/lxp/lxpCDrwRes.cpp"),
    (0x00205000, "src/engine/lxp/lxpCMemObj.cpp"),
    (0x00208000, "src/engine/lxp/lxpCResource.cpp"),
    (0x0020B000, "src/engine/lxp/lxpCResourceManagerBase.cpp"),
    (0x0020E000, "src/engine/lxp/lxpCResourceManagerRef.cpp"),
    (0x00211000, "src/engine/lxp/lxpCArchiveFA.cpp"),
    (0x00214000, "src/engine/lxp/lxpCArchiveXPCK.cpp"),
    (0x00217000, "src/engine/lxp/lxpCMath.cpp"),
    (0x0021A000, "src/engine/lxp/lxpCMatrix.cpp"),
    (0x0021D000, "src/engine/lxp/lxpCQuaternion.cpp"),
    (0x00220000, "src/engine/lxp/lxpCVector.cpp"),
    (0x00223000, "src/engine/lxp/lxpCGeometry.cpp"),
    (0x00226000, "src/engine/lxp/lxpCCollision.cpp"),
    (0x00229000, "src/engine/lxp/lxpCShader.cpp"),
    (0x0022C000, "src/engine/lxp/lxpCTexture.cpp"),
    (0x0022F000, "src/engine/lxp/lxpCRenderTarget.cpp"),
    (0x00232500, "src/engine/lxp/lxpCOpenGLES.cpp"),
    (0x00235500, "src/engine/lxp/lxpCCommandBuffer.cpp"),
    (0x00238500, "src/engine/lxp/lxpCResChara.cpp"),
    (0x0023B500, "src/engine/lxp/lxpCResModel.cpp"),
    (0x0023E500, "src/engine/lxp/lxpCSignature.cpp"),
    (0x00241500, "src/engine/lxp/lxpCBGProcess.cpp"),
    (0x00244500, "src/engine/lxp/lxpCObjLocus1.cpp"),
    (0x00247500, "src/engine/lxp/lxpCObjLocus2.cpp"),
    (0x0024A500, "src/engine/lxp/lxpCObjLocus.cpp"),
    (0x0024D500, "src/engine/lxp/lxpCContainerCMB.cpp"),
    (0x00250500, "src/engine/lxp/lxpCResAnime.cpp"),
    (0x00253500, "src/engine/lxp/lxpCContainerXSWP.cpp"),
    (0x00255500, "src/engine/lxp/lxpCContainerXIMA.cpp"),
    (0x00257500, "src/engine/lxp/lxpCObjParticle.cpp"),
    (0x0025A500, "src/engine/lxp/lxpCResParticle.cpp"),
    (0x0025D500, "src/engine/lxp/lxpCMotion.cpp"),
    (0x00260000, "src/engine/lxp/lxpCSkeleton.cpp"),
]

_ENGINE_CORE_UNITS = [
    (0x0013C000, "src/engine/core/lxpCSoundDriver.cpp"),
    (0x0013EC00, "src/engine/core/lxpCSoundChannel.cpp"),
    (0x00142000, "src/engine/core/lfwCObjManager.cpp"),
    (0x00145500, "src/engine/core/lxpCCamera.cpp"),
    (0x00149000, "src/engine/core/lxpCDspPipe.cpp"),
    (0x0014D000, "src/engine/core/lxpCAudioStream.cpp"),
    (0x00151600, "src/engine/core/lxpCSystemInit.cpp"),
    (0x00156000, "src/engine/core/lxpCThread.cpp"),
    (0x0015A000, "src/engine/core/lxpCFileIO.cpp"),
    (0x0015E000, "src/engine/core/lxpCArchiveCWAT.cpp"),
    (0x00162000, "src/engine/core/lfwCSceneManager.cpp"),
    (0x00166000, "src/engine/core/lfwCTaskManager.cpp"),
    (0x0016A000, "src/engine/core/lfwCEventManager.cpp"),
    (0x0016E000, "src/engine/core/lfwCInputManager.cpp"),
    (0x00172000, "src/engine/core/lfwCTextManager.cpp"),
    (0x00176000, "src/engine/core/lfwCLayoutManager.cpp"),
    (0x0017A000, "src/engine/core/lfwCSpriteManager.cpp"),
    (0x0017E000, "src/engine/core/lfwCWindowManager.cpp"),
    (0x00182000, "src/engine/core/lfwCEffectManager.cpp"),
    (0x00186000, "src/engine/core/lfwCSoundManager.cpp"),
    (0x0018B000, "src/engine/core/lxpCSyncObject.cpp"),
    (0x00190000, "src/engine/gds/GDSParser.cpp"),
    (0x00195000, "src/engine/gds/GDSFix.cpp"),
    (0x0019A000, "src/engine/gds/GDSSwitch.cpp"),
    (0x0019F000, "src/engine/gds/GDSCfgBin.cpp"),
    (0x001A4000, "src/engine/gds/GDSScriptContext.cpp"),
    (0x001A9000, "src/engine/gds/GDSScriptVM.cpp"),
    (0x001AE000, "src/engine/gds/GDSNode.cpp"),
    (0x001B3000, "src/engine/gds/GDSTable.cpp"),
    (0x001B8000, "src/engine/core/lxpCObjAnime.cpp"),
    (0x001BD000, "src/engine/core/lxpCObjChara.cpp"),
    (0x001C2000, "src/engine/core/lxpCObjCharaMerge.cpp"),
    (0x001C7000, "src/engine/core/lxpCObjSimpleModel.cpp"),
    (0x001CC000, "src/engine/core/lxpCObjColor.cpp"),
    (0x001D1000, "src/engine/core/lxpCObjImage.cpp"),
    (0x001D6000, "src/engine/core/lxpCObjImage2.cpp"),
    (0x001DB000, "src/engine/core/lxpCObjCanvas.cpp"),
    (0x001E0000, "src/engine/core/lxpCObjMovie.cpp"),
    (0x001E5000, "src/engine/core/lxpCObjBlur.cpp"),
    (0x001EA000, "src/engine/core/lxpCObjDOF.cpp"),
    (0x001EF000, "src/engine/core/lxpCObjEasyDOF.cpp"),
    (0x001F4000, "src/engine/core/lxpCObjCapture.cpp"),
    (0x001F9000, "src/engine/core/lxpCMessage.cpp"),
    (0x001FE000, "src/engine/core/lxpCContainerXPRM.cpp"),
    (0x00200500, "src/engine/core/lxpCCameraDevice.cpp"),
    (0x00263500, "src/engine/core/lxpCStackAllocator.cpp"),
    (0x00266000, "src/engine/core/lxpCBGLoadManager.cpp"),
    (0x0026A000, "src/engine/core/lxpCFileStream.cpp"),
    (0x0026E000, "src/engine/core/lxpCDecompressor.cpp"),
    (0x00272000, "src/engine/core/lxpCPackageLoader.cpp"),
    (0x00275000, "src/engine/core/lxpCBGBuildManager.cpp"),
    (0x00279000, "src/engine/core/lxpCContainerXMTM.cpp"),
    (0x00280200, "src/engine/core/lxpCContainerXCMA.cpp"),
    (0x00585000, "src/engine/core/lfwCCamera_Dev.cpp"),
    (0x0058C000, "src/engine/core/lfwCCamera_Fix.cpp"),
    (0x00592000, "src/engine/core/lfwCCamera_Mtn.cpp"),
    (0x00598000, "src/engine/core/lfwCCamera_Dolly.cpp"),
    (0x0059F000, "src/engine/core/lxpCSkyManager.cpp"),
    (0x005A5000, "src/engine/core/lxpCAnimeManager.cpp"),
    (0x005B5000, "src/engine/core/lxpCCharaManager.cpp"),
    (0x005B8000, "src/engine/core/lxpCImageManager.cpp"),
    (0x005BE000, "src/engine/core/lxpCLightManager.cpp"),
    (0x005C4000, "src/engine/core/lxpCMovieManager.cpp"),
    (0x005CA000, "src/engine/core/lxpCParticleManager.cpp"),
    (0x005D1000, "src/engine/core/lfwCObj2D.cpp"),
    (0x005D9000, "src/engine/core/lfwCObj3D.cpp"),
    (0x005DF000, "src/engine/core/lxpCLcs1.cpp"),
    (0x005E3800, "src/engine/core/lxpCLcs2.cpp"),
    (0x005E5500, "src/engine/liz/MemBase.cpp"),
    (0x005E6000, "src/engine/liz/ResBaseLXP.cpp"),
    (0x005E7C00, "src/engine/liz/ResAnimeLXP.cpp"),
    (0x005E7E80, "src/engine/liz/ResCharaLXP.cpp"),
    (0x005E8100, "src/engine/liz/ResImageLXP.cpp"),
    (0x005E8500, "src/engine/liz/ResModelLXP.cpp"),
    (0x005EB000, "src/engine/liz/ResObj2DLXP.cpp"),
    (0x005ED000, "src/engine/liz/ResSkyLXP.cpp"),
    (0x005F2000, "src/engine/liz/IResource.cpp"),
    (0x005F8000, "src/engine/core/lfwCRenderer.cpp"),
    (0x00600000, "src/engine/core/lfwCShaderPipeline.cpp"),
    (0x00608000, "src/engine/core/lfwCPostEffect.cpp"),
    (0x00610000, "src/engine/core/lfwCCecIcon.cpp"),
    (0x00618000, "src/engine/core/lfwCUIWidget.cpp"),
    (0x0061D000, "src/engine/core/lfwCLoopBase.cpp"),
    (0x00622000, "src/engine/core/lxpCLoop.cpp"),
    (0x00626000, "src/engine/core/lfwCMessage.cpp"),
    (0x00628000, "src/engine/core/lfwCObjLine.cpp"),
    (0x0062B000, "src/engine/core/lfwCObjImage.cpp"),
    (0x00630000, "src/engine/core/lxpCResContainer.cpp"),
    (0x00634000, "src/engine/core/lfwCCamera_Move.cpp"),
    (0x00638000, "src/engine/core/lfwCCamera_Orbit.cpp"),
    (0x0063D000, "src/engine/core/lxpCCameraMotion.cpp"),
    (0x00644000, "src/engine/core/lxpCNetworkSession.cpp"),
    (0x0064C000, "src/engine/core/lfwCSaveDataManager.cpp"),
    (0x00654000, "src/engine/core/lfwCInventoryBase.cpp"),
    (0x0065C000, "src/engine/core/lfwCScriptBinding.cpp"),
    (0x00664000, "src/engine/core/lfwCPhysicsWorld.cpp"),
    (0x0066C000, "src/engine/core/lfwCCollisionSolver.cpp"),
    (0x00674000, "src/engine/core/lfwCMapLoader.cpp"),
    (0x0067A000, "src/engine/core/lfwCObjectVirtual.cpp"),
    (0x00682000, "src/engine/core/lxpCMemoryDevice.cpp"),
    (0x0068A000, "src/engine/core/lxpCUniformMatrix.cpp"),
    (0x00692000, "src/engine/core/lfwCMenuTransition.cpp"),
    (0x0069A000, "src/engine/core/lfwCMenuMatrix.cpp"),
    (0x006A4000, "src/engine/core/lfwCAnimationBlender.cpp"),
    (0x006AE000, "src/engine/core/lfwCParticleEmitter.cpp"),
    (0x006B8000, "src/engine/core/lfwCFontRenderer.cpp"),
    (0x006C2000, "src/engine/core/lfwCSoundPlayer.cpp"),
    (0x006CC000, "src/engine/core/lfwCMoviePlayer.cpp"),
    (0x006D6000, "src/engine/core/lfwCDebugMenu.cpp"),
    (0x006E0000, "src/engine/core/lfwCMathUtil.cpp"),
    (0x006EA000, "src/engine/core/lfwCStringUtil.cpp"),
    (0x00700000, "src/engine/core/lfwCSystemRuntime.cpp"),
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

    # 2. CTR: nn::nex & networking classes (ACNL layout)
    if subsys == "nn_nex":
        m = re.match(r"^nn::nex::([A-Za-z0-9]+)::", dem)
        if m:
            return f"src/ctr/nn/nex/{m.group(1)}.cpp"
        if vma == 0x00301248:
            return "src/ctr/nn/nex/nex_RootObject.cpp"
        return _lookup_range_unit(vma, _ACNL_NEX_UNITS, "src/ctr/nn/nex/nex_BackEndServices.cpp")

    # 3. CTR: standard subsystems (ACNL layout)
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
        return _lookup_range_unit(vma, _ACNL_CTR_SERVICE_UNITS, "src/ctr/nn/cec/cec_CecControl.cpp")

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
                return "src/engine/lfw/lxpCResourceManager.cpp"
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
        return _lookup_range_unit(vma, _ENGINE_CORE_UNITS, "src/engine/core/lfwCSystemRuntime.cpp")

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

    # 1. Unit: extracted_data (matching Animal Crossing reference layout)
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
