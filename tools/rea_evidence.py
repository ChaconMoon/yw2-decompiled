#!/usr/bin/env python3
"""
tools/rea_evidence.py — Evidence-Based Reverse Engineering Inspector (REA Model)
for Yo-kai Watch 2: Psychic Specters (Nintendo 3DS / ARMv6K).

Implements the 4-pillar REA (Reverse Engineer Anything) contract directly on
extracted/code.raw.bin, config/memory_map.json, and the modular translation units:
  1. Evidence     : Verified binary facts (VMA, literal pool, strings, RTTI vtables,
                    3DS TLS/IPC command headers, kernel SVCs).
  2. Graph        : Exact incoming callers (XREFs In), outgoing callees (XREFs Out),
                    and Bottom-Up readiness status.
  3. Observations : Struct field offsets ([r0, #0x...]) with inferred widths (u8/u16/u32/u64),
                    register usage, and didactic difficulty level (L1..L4).
  4. Limitations  : Unmatched callees, unnamed globals in .data/.bss, and indirect calls.

Usage:
  python3 tools/rea_evidence.py --func 0x001389CC
  python3 tools/rea_evidence.py --unit src/ctr/nn/dsp/CTR_Api.cpp
  python3 tools/rea_evidence.py --unit lfwCLoop.cpp --json
"""

import argparse
from bisect import bisect_right
import json
from pathlib import Path
import re
import struct
import subprocess
import sys

PROJECT_ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_ROOT / "tools"))

import generate_report  # noqa: E402

BINARY_PATH = PROJECT_ROOT / "extracted" / "code.raw.bin"
BASE_VMA = 0x00100000
TEXT_END = 0x006F32AC
RODATA_START = 0x006F4000
RODATA_END = 0x0080C42C
DATA_START = 0x0080D000
DATA_END = 0x00860934
BSS_START = 0x00860934
BSS_END = 0x0091B9BC

SVC_NAMES = {
    0x01: "svcControlMemory",
    0x02: "svcQueryMemory",
    0x03: "svcExitProcess",
    0x08: "svcCreateThread",
    0x09: "svcExitThread",
    0x0A: "svcSleepThread",
    0x0B: "svcGetThreadPriority",
    0x0C: "svcSetThreadPriority",
    0x13: "svcCreateMutex",
    0x14: "svcReleaseMutex",
    0x15: "svcCreateSemaphore",
    0x16: "svcReleaseSemaphore",
    0x17: "svcCreateEvent",
    0x18: "svcSignalEvent",
    0x19: "svcClearEvent",
    0x1A: "svcCreateTimer",
    0x1B: "svcSetTimer",
    0x1C: "svcCancelTimer",
    0x1D: "svcClearTimer",
    0x1E: "svcCreateMemoryBlock",
    0x1F: "svcMapMemoryBlock",
    0x20: "svcUnmapMemoryBlock",
    0x21: "svcCreateAddressArbiter",
    0x22: "svcArbitrateAddress",
    0x23: "svcCloseHandle",
    0x24: "svcWaitSynchronization1",
    0x25: "svcWaitSynchronizationN",
    0x27: "svcDuplicateHandle",
    0x28: "svcGetSystemTick",
    0x29: "svcGetHandleInfo",
    0x2A: "svcGetSystemInfo",
    0x2B: "svcGetProcessInfo",
    0x2C: "svcGetThreadInfo",
    0x2D: "svcConnectToPort",
    0x32: "svcSendSyncRequest",
    0x33: "svcOpenProcess",
    0x35: "svcGetProcessId",
    0x37: "svcGetThreadId",
    0x38: "svcGetResourceLimit",
    0x39: "svcGetResourceLimitLimitValues",
    0x3A: "svcGetResourceLimitCurrentValues",
    0x3C: "svcBreak",
    0x3D: "svcOutputDebugString",
}


def classify_vma_section(vma: int) -> str:
    if BASE_VMA <= vma < TEXT_END:
        return ".text"
    if RODATA_START <= vma < RODATA_END:
        return ".rodata"
    if DATA_START <= vma < DATA_END:
        return ".data"
    if BSS_START <= vma < BSS_END:
        return ".bss"
    if 0x1FF80000 <= vma <= 0x1FF81FFF:
        return "SharedPage (Kernel Config/Time)"
    return "constant/external"


def read_string_at_vma(data: bytes, vma: int, max_len: int = 80) -> str | None:
    if not (BASE_VMA <= vma < DATA_END):
        return None
    off = vma - BASE_VMA
    end = data.find(b"\x00", off, min(len(data), off + max_len))
    if end == -1 or end - off < 3:
        return None
    chunk = data[off:end]
    if all(32 <= b < 127 for b in chunk) and any(chr(b).isalpha() for b in chunk):
        return chunk.decode("ascii", errors="ignore")
    return None


def build_rtti_vtable_index(data: bytes) -> dict[int, list[dict]]:
    """Maps function VMAs to any RTTI vtables in .rodata/.data that reference them."""
    strings = {}
    ro_slice = data[RODATA_START - BASE_VMA : DATA_END - BASE_VMA]
    for m in re.finditer(
        rb"((?:N[0-9]+[A-Za-z0-9_]+E|[0-9]{1,2}[A-Za-z_$][A-Za-z0-9_$]*))\x00",
        ro_slice,
    ):
        if m.start() == 0 or ro_slice[m.start() - 1] == 0:
            s = m.group(1).decode("ascii", errors="ignore")
            strings[RODATA_START + m.start(1)] = s

    ti_map = {}
    for off in range(RODATA_START - BASE_VMA, DATA_END - BASE_VMA - 4, 4):
        val = struct.unpack_from("<I", data, off + 4)[0]
        if val in strings:
            vt = struct.unpack_from("<I", data, off)[0]
            if RODATA_START <= vt < DATA_END:
                ti_map[BASE_VMA + off] = strings[val]

    fn_vtable_refs: dict[int, list[dict]] = {}
    for off in range(RODATA_START - BASE_VMA, DATA_END - BASE_VMA - 8, 4):
        ti_ptr = struct.unpack_from("<I", data, off)[0]
        if ti_ptr in ti_map:
            cls_name = ti_map[ti_ptr]
            vt_vma = BASE_VMA + off + 4
            for slot in range(64):
                entry_off = off + 4 + slot * 4
                if entry_off + 4 > DATA_END - BASE_VMA:
                    break
                fn_raw = struct.unpack_from("<I", data, entry_off)[0]
                if fn_raw == 0:
                    continue
                fn_clean = fn_raw & ~1
                if BASE_VMA <= fn_clean < TEXT_END:
                    fn_vtable_refs.setdefault(fn_clean, []).append(
                        {
                            "rtti_class": cls_name,
                            "vtable_vma": f"0x{vt_vma:08X}",
                            "slot": slot,
                            "slot_offset": f"0x{slot * 4:02X}",
                        }
                    )
                else:
                    break
    return fn_vtable_refs


def decode_arm_branch_target(vma: int, word: int) -> tuple[str, int] | None:
    """Decodes ARM B / BL / BLX(imm) instructions and returns (mnemonic, target_vma)."""
    cond = (word >> 28) & 0xF
    op = (word >> 24) & 0xF
    if cond == 0xF:
        if op in (0xA, 0xB):
            h = (word >> 24) & 1
            imm24 = word & 0xFFFFFF
            if imm24 & 0x800000:
                imm24 -= 0x1000000
            target = (vma + 8 + (imm24 << 2) + (h << 1)) & 0xFFFFFFFF
            return ("blx", target)
        return None
    if op == 0xB:
        imm24 = word & 0xFFFFFF
        if imm24 & 0x800000:
            imm24 -= 0x1000000
        target = (vma + 8 + (imm24 << 2)) & 0xFFFFFFFF
        return ("bl", target)
    if op == 0xA:
        imm24 = word & 0xFFFFFF
        if imm24 & 0x800000:
            imm24 -= 0x1000000
        target = (vma + 8 + (imm24 << 2)) & 0xFFFFFFFF
        return ("b", target)
    return None


def analyze_function_instructions(
    data: bytes, func_vma: int, func_size: int, func_index: dict[int, dict]
) -> dict:
    """Extracts literal pool entries, IPC headers, SVCs, struct accesses, and outgoing calls."""
    start_off = func_vma - BASE_VMA
    end_off = start_off + func_size

    # First pass: find PC-relative loads (ldr rX, [pc, #imm]) to identify literal pool boundary
    pool_addrs = set()
    code_limit_off = end_off
    for off in range(start_off, end_off, 4):
        word = struct.unpack_from("<I", data, off)[0]
        vma = BASE_VMA + off
        # ARM LDR Rd, [PC, #imm12] -> cond != 0xF, bits 27..20 == 01011001 (0x59) or 01010001 (0x51)
        if (word >> 28) != 0xF and ((word >> 20) & 0xF7) == 0x51 and ((word >> 16) & 0xF) == 15:
            up = (word >> 23) & 1
            imm12 = word & 0xFFF
            target_vma = (vma + 8 + (imm12 if up else -imm12)) & ~3
            if func_vma <= target_vma < func_vma + func_size:
                pool_addrs.add(target_vma)
                pool_off = target_vma - BASE_VMA
                if pool_off < code_limit_off:
                    code_limit_off = pool_off

    callees = []
    seen_callees = set()
    svcs = []
    uses_tls = False
    indirect_calls = []
    struct_accesses = []
    embedded_strings = []

    for off in range(start_off, code_limit_off, 4):
        word = struct.unpack_from("<I", data, off)[0]
        vma = BASE_VMA + off

        # Check TLS access: mrc p15, 0, Rd, c13, c0, 3 -> 0xEE1D?F70
        if (word & 0x0FF0FFF) == 0x0E1D0F70:
            uses_tls = True

        # Check SVC: cond != 0xF, bits 27..24 == 0xF
        if (word >> 28) != 0xF and ((word >> 24) & 0xF) == 0xF:
            svc_id = word & 0xFF
            svcs.append(
                {
                    "at_vma": f"0x{vma:08X}",
                    "svc_id": f"0x{svc_id:02X}",
                    "name": SVC_NAMES.get(svc_id, f"svc_{svc_id:02X}"),
                }
            )

        # Check BLX Rm (indirect call): bits 27..4 == 0x012FFF3
        if (word >> 28) != 0xF and ((word >> 4) & 0x0FFFFFF) == 0x012FFF3:
            rm = word & 0xF
            indirect_calls.append({"at_vma": f"0x{vma:08X}", "register": f"r{rm}"})

        # Check B / BL
        br = decode_arm_branch_target(vma, word)
        if br:
            mnem, target = br
            if not (func_vma <= target < func_vma + func_size):
                if target not in seen_callees and BASE_VMA <= target < TEXT_END:
                    seen_callees.add(target)
                    callee_fn = func_index.get(target)
                    callees.append(
                        {
                            "at_vma": f"0x{vma:08X}",
                            "mnemonic": mnem,
                            "target_vma": f"0x{target:08X}",
                            "symbol": callee_fn["symbol"] if callee_fn else f"func_{target:08X}",
                            "unit": callee_fn["unit"] if callee_fn else "unknown",
                            "status": callee_fn["status"] if callee_fn else "unmatched",
                        }
                    )

        # Check struct field loads/stores on r0..r3:
        # Single data transfer (LDR/STR/LDRB/STRB): bits 27..26 == 0b01, I=0, P=1, U=1, W=0
        if (word >> 28) != 0xF and ((word >> 26) & 0x3) == 1 and ((word >> 25) & 1) == 0:
            p_bit = (word >> 24) & 1
            u_bit = (word >> 23) & 1
            b_bit = (word >> 22) & 1
            w_bit = (word >> 21) & 1
            l_bit = (word >> 20) & 1
            rn = (word >> 16) & 0xF
            rd = (word >> 12) & 0xF
            imm12 = word & 0xFFF
            if p_bit == 1 and u_bit == 1 and w_bit == 0 and rn in (0, 1, 2, 3):
                struct_accesses.append(
                    {
                        "at_vma": f"0x{vma:08X}",
                        "base_reg": f"r{rn}",
                        "offset": f"0x{imm12:02X}",
                        "offset_int": imm12,
                        "access": "read" if l_bit else "write",
                        "type": "u8" if b_bit else "u32",
                        "dest_reg": f"r{rd}",
                    }
                )
        # Halfword / Signed / Doubleword transfer (LDRH/STRH/LDRSB/LDRSH/LDRD/STRD)
        elif (
            (word >> 28) != 0xF
            and ((word >> 25) & 0x7) == 0
            and ((word >> 7) & 1) == 1
            and ((word >> 4) & 1) == 1
            and ((word >> 22) & 1) == 1  # immediate offset
        ):
            p_bit = (word >> 24) & 1
            u_bit = (word >> 23) & 1
            w_bit = (word >> 21) & 1
            l_bit = (word >> 20) & 1
            rn = (word >> 16) & 0xF
            rd = (word >> 12) & 0xF
            sh = (word >> 5) & 0x3
            imm8 = ((word >> 4) & 0xF0) | (word & 0xF)
            if p_bit == 1 and u_bit == 1 and w_bit == 0 and rn in (0, 1, 2, 3) and sh != 0:
                if l_bit == 0 and sh == 2:
                    acc_type, acc_mode = "u64", "read"
                elif l_bit == 0 and sh == 3:
                    acc_type, acc_mode = "u64", "write"
                elif sh == 1:
                    acc_type, acc_mode = "u16", ("read" if l_bit else "write")
                elif sh == 2:
                    acc_type, acc_mode = "s8", "read"
                else:
                    acc_type, acc_mode = "s16", "read"
                struct_accesses.append(
                    {
                        "at_vma": f"0x{vma:08X}",
                        "base_reg": f"r{rn}",
                        "offset": f"0x{imm8:02X}",
                        "offset_int": imm8,
                        "access": acc_mode,
                        "type": acc_type,
                        "dest_reg": f"r{rd}",
                    }
                )

    # Inspect literal pool entries
    literals = []
    ipc_headers = []
    for pool_vma in sorted(pool_addrs):
        val = struct.unpack_from("<I", data, pool_vma - BASE_VMA)[0]
        sec = classify_vma_section(val)
        str_val = read_string_at_vma(data, val)
        entry = {
            "pool_vma": f"0x{pool_vma:08X}",
            "value": f"0x{val:08X}",
            "section": sec,
        }
        if str_val:
            entry["string"] = str_val
        # Check if val is a 3DS IPC header when function uses TLS
        hi = val >> 16
        norm = (val >> 6) & 0x3F
        trans = val & 0x3F
        if uses_tls and (1 <= hi <= 0x60 or 0x0401 <= hi <= 0x0860) and norm <= 16 and trans <= 16:
            ipc_info = {
                "header": f"0x{val:08X}",
                "command_id": f"0x{hi:04X}",
                "normal_params": norm,
                "translate_params": trans,
            }
            entry["ipc_header"] = ipc_info
            ipc_headers.append(ipc_info)
        literals.append(entry)

    # Check for embedded strings at end of function (.text pool)
    if code_limit_off < end_off:
        pool_bytes = data[code_limit_off:end_off]
        for m in re.finditer(rb"([A-Za-z_][A-Za-z0-9_:.\- ]{4,})\x00", pool_bytes):
            s = m.group(1).decode("ascii", errors="ignore")
            if sum(c.isalpha() for c in s) >= 4:
                embedded_strings.append(
                    {"vma": f"0x{BASE_VMA + code_limit_off + m.start(1):08X}", "string": s}
                )

    return {
        "code_bytes": code_limit_off - start_off,
        "pool_bytes": end_off - code_limit_off,
        "uses_tls": uses_tls,
        "ipc_headers": ipc_headers,
        "svcs": svcs,
        "literals": literals,
        "embedded_strings": embedded_strings,
        "callees": callees,
        "indirect_calls": indirect_calls,
        "struct_accesses": struct_accesses,
    }


def scan_incoming_xrefs(
    data: bytes, target_vma: int, sorted_vmas: list[int], func_index: dict[int, dict]
) -> list[dict]:
    """Scans .text for all ARM B/BL instructions targeting target_vma."""
    callers = []
    text_len = TEXT_END - BASE_VMA
    for off in range(0, text_len, 4):
        word = struct.unpack_from("<I", data, off)[0]
        op = (word >> 24) & 0xF
        if op not in (0xA, 0xB):
            continue
        vma = BASE_VMA + off
        br = decode_arm_branch_target(vma, word)
        if br and br[1] == target_vma:
            idx = bisect_right(sorted_vmas, vma) - 1
            caller_fn = func_index.get(sorted_vmas[idx]) if idx >= 0 else None
            if caller_fn and caller_fn["address"] == target_vma:
                continue
            callers.append(
                {
                    "at_vma": f"0x{vma:08X}",
                    "mnemonic": br[0],
                    "caller_vma": f"0x{caller_fn['address']:08X}" if caller_fn else "unknown",
                    "caller_symbol": caller_fn["symbol"] if caller_fn else f"func_{vma:08X}",
                    "caller_unit": caller_fn["unit"] if caller_fn else "unknown",
                    "caller_status": caller_fn["status"] if caller_fn else "unmatched",
                }
            )
    return callers


def build_function_report(
    fn: dict,
    data: bytes,
    sorted_vmas: list[int],
    func_index: dict[int, dict],
    vtable_index: dict[int, list[dict]],
) -> dict:
    vma = fn["address"]
    size = fn["size"]
    inst_info = analyze_function_instructions(data, vma, size, func_index)
    callers = scan_incoming_xrefs(data, vma, sorted_vmas, func_index)
    vtable_refs = vtable_index.get(vma, [])

    matched_statuses = ("matched", "community_matched", "matched_community")
    unmatched_callees = [
        c for c in inst_info["callees"] if c["status"] not in matched_statuses
    ]
    unnamed_globals = [
        lit
        for lit in inst_info["literals"]
        if lit["section"] in (".data", ".bss", ".rodata") and "string" not in lit
    ]

    # Disassembly via dump_asm.py
    asm_res = subprocess.run(
        [sys.executable, str(PROJECT_ROOT / "tools" / "dump_asm.py"), f"0x{vma:08X}", "--plain"],
        capture_output=True,
        text=True,
        check=False,
    )
    asm_text = asm_res.stdout.strip() if asm_res.returncode == 0 else ""

    return {
        "schema": "rea-3ds-evidence-v1",
        "target": {
            "symbol": fn["symbol"],
            "vma": f"0x{vma:08X}",
            "size_bytes": size,
            "code_bytes": inst_info["code_bytes"],
            "pool_bytes": inst_info["pool_bytes"],
            "layer": fn["layer"],
            "subsystem": fn["subsystem"],
            "unit": fn["unit"],
            "status": fn["status"],
        },
        "evidence": {
            "vtable_rtti_refs": vtable_refs,
            "uses_3ds_tls": inst_info["uses_tls"],
            "ipc_headers": inst_info["ipc_headers"],
            "kernel_svcs": inst_info["svcs"],
            "literal_pool": inst_info["literals"],
            "embedded_strings": inst_info["embedded_strings"],
            "disassembly": asm_text,
        },
        "graph": {
            "incoming_callers_count": len(callers),
            "incoming_callers": callers,
            "outgoing_callees_count": len(inst_info["callees"]),
            "outgoing_callees": inst_info["callees"],
            "bottom_up_ready": len(unmatched_callees) == 0,
        },
        "observations": {
            "didactic_level": (
                "L1 (Leaf / Trivial, <=24B)"
                if size <= 24 and not inst_info["callees"]
                else "L2 (Accessor / Arithmetic, <=48B)"
                if size <= 48
                else "L3 (Control Flow / Helper Caller, <=96B)"
                if size <= 96
                else "L4 (Complex Method / State Machine, >96B)"
            ),
            "struct_field_accesses": inst_info["struct_accesses"],
        },
        "limitations": {
            "unmatched_callees": unmatched_callees,
            "unnamed_globals": unnamed_globals,
            "indirect_register_calls": inst_info["indirect_calls"],
        },
    }


def print_function_markdown(rep: dict) -> None:
    t = rep["target"]
    ev = rep["evidence"]
    gr = rep["graph"]
    ob = rep["observations"]
    lim = rep["limitations"]

    print("=" * 72)
    print(f"  REA 3DS Evidence Report — {t['symbol']} ({t['vma']})")
    print("=" * 72)
    print(f"  • Unidad C/C++ : {t['unit']}")
    print(f"  • Capa/Módulo  : {t['layer']}/{t['subsystem']}  |  Estado: {t['status']}")
    print(
        f"  • Tamaño       : {t['size_bytes']} B ({t['code_bytes']} B código + {t['pool_bytes']} B literal pool)"
    )
    print(f"  • Nivel        : {ob['didactic_level']}")
    ready_str = "✅ SÍ (0 subfunciones pendientes)" if gr["bottom_up_ready"] else f"⏳ NO ({len(lim['unmatched_callees'])} subfunciones pendientes)"
    print(f"  • Bottom-Up    : {ready_str}")
    print("-" * 72)

    print("\n[1] EVIDENCE (Hechos Verificados en el Binario)")
    if ev["vtable_rtti_refs"]:
        for vt in ev["vtable_rtti_refs"]:
            print(
                f"  • Vtable RTTI  : Clase '{vt['rtti_class']}' @ {vt['vtable_vma']} [slot #{vt['slot']} (+{vt['slot_offset']})]"
            )
    if ev["uses_3ds_tls"]:
        print("  • Kernel TLS   : Accede a Thread Local Storage (mrc p15, 0, rX, c13, c0, 3)")
    for ipc in ev["ipc_headers"]:
        print(
            f"  • Cabecera IPC : {ipc['header']} -> CmdID={ipc['command_id']}, NormalParams={ipc['normal_params']}, TranslateParams={ipc['translate_params']}"
        )
    for svc in ev["kernel_svcs"]:
        print(f"  • Syscall SVC  : {svc['name']} ({svc['svc_id']}) @ {svc['at_vma']}")
    for lit in ev["literal_pool"]:
        extra = f' -> "{lit["string"]}"' if "string" in lit else ""
        print(f"  • Literal Pool : [{lit['pool_vma']}] = {lit['value']} ({lit['section']}){extra}")
    for emb in ev["embedded_strings"]:
        print(f'  • Cadena .text : [{emb["vma"]}] = "{emb["string"]}"')

    print("\n[2] GRAPH / XREFs (Relaciones de Llamada)")
    print(f"  • Callers entrantes ({gr['incoming_callers_count']}):")
    for c in gr["incoming_callers"][:10]:
        print(
            f"      <- {c['caller_symbol']} ({c['caller_vma']}) en {c['caller_unit']} [{c['caller_status']}]"
        )
    if gr["incoming_callers_count"] > 10:
        print(f"      ... (+{gr['incoming_callers_count'] - 10} llamadas adicionales)")
    print(f"  • Callees salientes ({gr['outgoing_callees_count']}):")
    for c in gr["outgoing_callees"]:
        mark = "✅" if c["status"] in ("matched", "community_matched", "matched_community") else "⏳"
        print(
            f"      -> {mark} {c['mnemonic']} {c['symbol']} ({c['target_vma']}) en {c['unit']} [{c['status']}]"
        )

    print("\n[3] OBSERVATIONS (Accesos a Structs y Registros)")
    if ob["struct_field_accesses"]:
        for acc in ob["struct_field_accesses"]:
            print(
                f"  • [{acc['base_reg']} + {acc['offset']}] ({acc['type']}, {acc['access']}) <-> {acc['dest_reg']} @ {acc['at_vma']}"
            )
    else:
        print("  • Sin accesos con desplazamiento inmediato sobre r0..r3.")

    print("\n[4] LIMITATIONS / UNKNOWNS (Pendientes de Curaduría)")
    if lim["unmatched_callees"]:
        print(
            f"  • Subfunciones sin decompilar ({len(lim['unmatched_callees'])}): "
            + ", ".join(f"{c['symbol']} ({c['target_vma']})" for c in lim["unmatched_callees"])
        )
    if lim["unnamed_globals"]:
        print(
            f"  • Punteros a secciones de datos ({len(lim['unnamed_globals'])}): "
            + ", ".join(f"{g['value']} ({g['section']})" for g in lim["unnamed_globals"])
        )
    if lim["indirect_register_calls"]:
        print(
            "  • Llamadas indirectas/virtuales (BLX): "
            + ", ".join(f"{ic['register']} @ {ic['at_vma']}" for ic in lim["indirect_register_calls"])
        )
    if not (lim["unmatched_callees"] or lim["unnamed_globals"] or lim["indirect_register_calls"]):
        print("  • Ninguna incógnita externa: función autocontenida.")
    print("=" * 72)


def build_unit_report(
    unit_query: str,
    all_funcs: list[dict],
    data: bytes,
    func_index: dict[int, dict],
    vtable_index: dict[int, list[dict]],
) -> dict:
    q_low = unit_query.lower()
    all_units = sorted({f["unit"] for f in all_funcs})
    exact_units = [
        u
        for u in all_units
        if u.lower() == q_low
        or Path(u).name.lower() == q_low
        or Path(u).stem.lower() == q_low
    ]
    matched_units = exact_units or [u for u in all_units if q_low in u.lower()]
    if not matched_units:
        sys.exit(f"❌ No se encontró ninguna unidad que coincida con '{unit_query}'.")
    target_unit = matched_units[0]
    unit_funcs = [f for f in all_funcs if f["unit"] == target_unit]

    matched_statuses = ("matched", "community_matched", "matched_community")
    matched_fns = [f for f in unit_funcs if f["status"] in matched_statuses]
    unmatched_fns = [f for f in unit_funcs if f["status"] not in matched_statuses]

    # Analyze all functions in the unit to build the Bottom-Up queue and Struct Offset Aggregator
    bottom_up_candidates = []
    struct_r0_offsets: dict[int, dict] = {}

    for fn in unit_funcs:
        vma = fn["address"]
        size = fn["size"]
        info = analyze_function_instructions(data, vma, size, func_index)
        for acc in info["struct_accesses"]:
            if acc["base_reg"] == "r0":
                off_int = acc["offset_int"]
                slot = struct_r0_offsets.setdefault(
                    off_int,
                    {"offset": f"0x{off_int:02X}", "types": set(), "modes": set(), "funcs": set()},
                )
                slot["types"].add(acc["type"])
                slot["modes"].add(acc["access"])
                slot["funcs"].add(fn["symbol"])

        if fn["status"] not in matched_statuses:
            unmatched_callees = [
                c for c in info["callees"] if c["status"] not in matched_statuses
            ]
            vt_refs = vtable_index.get(vma, [])
            bottom_up_candidates.append(
                {
                    "symbol": fn["symbol"],
                    "vma": f"0x{vma:08X}",
                    "size_bytes": size,
                    "unmatched_callees_count": len(unmatched_callees),
                    "total_callees_count": len(info["callees"]),
                    "bottom_up_ready": len(unmatched_callees) == 0,
                    "uses_tls_ipc": info["uses_tls"] and len(info["ipc_headers"]) > 0,
                    "vtable_slot": vt_refs[0]["slot"] if vt_refs else None,
                    "rtti_class": vt_refs[0]["rtti_class"] if vt_refs else None,
                }
            )

    bottom_up_candidates.sort(
        key=lambda c: (c["unmatched_callees_count"], c["size_bytes"], int(c["vma"], 16))
    )

    aggregated_offsets = []
    for off_int in sorted(struct_r0_offsets):
        item = struct_r0_offsets[off_int]
        aggregated_offsets.append(
            {
                "offset": item["offset"],
                "inferred_types": sorted(item["types"]),
                "access_modes": sorted(item["modes"]),
                "function_count": len(item["funcs"]),
                "sample_functions": sorted(item["funcs"])[:5],
            }
        )

    total_bytes = sum(f["size"] for f in unit_funcs)
    matched_bytes = sum(f["size"] for f in matched_fns)

    return {
        "schema": "rea-3ds-unit-evidence-v1",
        "unit": target_unit,
        "progress": {
            "matched_functions": len(matched_fns),
            "total_functions": len(unit_funcs),
            "percent_functions": round(len(matched_fns) * 100.0 / max(1, len(unit_funcs)), 2),
            "matched_bytes": matched_bytes,
            "total_bytes": total_bytes,
            "percent_bytes": round(matched_bytes * 100.0 / max(1, total_bytes), 2),
        },
        "bottom_up_queue": bottom_up_candidates,
        "struct_r0_layout_evidence": aggregated_offsets,
    }


def print_unit_markdown(rep: dict) -> None:
    p = rep["progress"]
    print("=" * 72)
    print(f"  REA 3DS Unit Evidence & Bottom-Up Planner — {rep['unit']}")
    print("=" * 72)
    print(
        f"  • Progreso Funciones : {p['matched_functions']} / {p['total_functions']} ({p['percent_functions']}%)"
    )
    print(
        f"  • Progreso Bytes     : {p['matched_bytes']} / {p['total_bytes']} B ({p['percent_bytes']}%)"
    )
    ready_count = sum(1 for c in rep["bottom_up_queue"] if c["bottom_up_ready"])
    print(
        f"  • Hojas Bottom-Up    : {ready_count} funciones listas ahora mismo (0 dependencias sin decompilar)"
    )
    print("-" * 72)

    print("\n[1] COLA BOTTOM-UP RECOMENDADA (De Hojas Simples a Ramas)")
    if not rep["bottom_up_queue"]:
        print("  🎉 ¡Unidad 100% completada!")
    else:
        for i, c in enumerate(rep["bottom_up_queue"][:15], 1):
            badge = "✅ READY" if c["bottom_up_ready"] else f"⏳ Wait({c['unmatched_callees_count']} deps)"
            vt_tag = f" [vtable #{c['vtable_slot']} {c['rtti_class']}]" if c["vtable_slot"] is not None else ""
            ipc_tag = " [IPC]" if c["uses_tls_ipc"] else ""
            print(
                f"  {i:2d}. {badge:14s} | {c['symbol']:18s} ({c['vma']}) | {c['size_bytes']:4d} B | Callees: {c['total_callees_count']}{ipc_tag}{vt_tag}"
            )
        if len(rep["bottom_up_queue"]) > 15:
            print(f"  ... (+{len(rep['bottom_up_queue']) - 15} funciones pendientes en esta unidad)")

    print("\n[2] EVIDENCIA AGREGADA DE LAYOUT DE STRUCT (Accesos sobre [r0, #0x...])")
    if not rep["struct_r0_layout_evidence"]:
        print("  • Sin accesos directos a campos de 'r0' en esta unidad.")
    else:
        for field in rep["struct_r0_layout_evidence"][:25]:
            types_str = "/".join(field["inferred_types"])
            modes_str = "/".join(field["access_modes"])
            fns_str = ", ".join(field["sample_functions"])
            print(
                f"  • +{field['offset']:6s} : {types_str:8s} ({modes_str:10s}) — usado en {field['function_count']:2d} fn(s) [{fns_str}]"
            )
        if len(rep["struct_r0_layout_evidence"]) > 25:
            print(f"  ... (+{len(rep['struct_r0_layout_evidence']) - 25} offsets adicionales)")
    print("=" * 72)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="REA-style Evidence-Based Reverse Engineering Inspector for Yo-kai Watch 2 (3DS)"
    )
    parser.add_argument("--func", help="Function address (e.g. 0x001389CC) or symbol name")
    parser.add_argument("--unit", help="Target C/C++ unit path or name (e.g. CTR_Api.cpp)")
    parser.add_argument("--json", action="store_true", help="Emit structured JSON output")
    args = parser.parse_args()

    if not args.func and not args.unit:
        parser.error("Especifica --func <0xADDRESS|symbol> o --unit <path.cpp>")

    if not BINARY_PATH.exists():
        sys.exit(f"❌ No se encontró {BINARY_PATH}. Ejecuta 'make extract' primero.")

    data = BINARY_PATH.read_bytes()
    all_funcs = generate_report.load_all_functions()
    func_index = {}
    for f in all_funcs:
        f["unit"] = generate_report.assign_target_unit(f)
        func_index[f["address"]] = f
    sorted_vmas = sorted(func_index.keys())
    vtable_index = build_rtti_vtable_index(data)

    if args.func:
        q = args.func.strip()
        target_fn = None
        if q.lower().startswith("0x") or all(c in "0123456789abcdefABCDEF" for c in q):
            vma = int(q, 16)
            target_fn = func_index.get(vma)
        if not target_fn:
            for f in all_funcs:
                if f["symbol"] == q or q.lower() in f["symbol"].lower():
                    target_fn = f
                    break
        if not target_fn:
            sys.exit(f"❌ Función '{q}' no encontrada en el catálogo.")

        rep = build_function_report(target_fn, data, sorted_vmas, func_index, vtable_index)
        if args.json:
            print(json.dumps(rep, indent=2, ensure_ascii=False))
        else:
            print_function_markdown(rep)
    elif args.unit:
        rep = build_unit_report(args.unit.strip(), all_funcs, data, func_index, vtable_index)
        if args.json:
            print(json.dumps(rep, indent=2, ensure_ascii=False))
        else:
            print_unit_markdown(rep)


if __name__ == "__main__":
    main()
