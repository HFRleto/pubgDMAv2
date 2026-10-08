#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from __future__ import annotations

import argparse
import ctypes
import hashlib
import json
import struct
import sys
import time
from pathlib import Path
from typing import Optional, List, Dict, Tuple

from iced_x86 import Decoder, OpKind, Mnemonic, Register
import memprocfs

# ---------------------------------------------------------------------------
# ANSI COLORS
# ---------------------------------------------------------------------------

def _enable_ansi() -> None:
    if sys.platform == "win32":
        try:
            kernel32 = ctypes.windll.kernel32
            handle = kernel32.GetStdHandle(-11)
            mode = ctypes.c_uint32()
            kernel32.GetConsoleMode(handle, ctypes.byref(mode))
            kernel32.SetConsoleMode(handle, mode.value | 0x0004)
        except Exception:
            pass

_enable_ansi()

class C:
    GREEN = "\033[92m"
    RED = "\033[91m"
    YELLOW = "\033[93m"
    BLUE = "\033[94m"
    CYAN = "\033[96m"
    BOLD = "\033[1m"
    RESET = "\033[0m"

def log(msg: str) -> None:
    print(msg, flush=True)

def log_info(msg: str) -> None:
    print(f"{C.CYAN}{msg}{C.RESET}", flush=True)

def log_ok(msg: str) -> None:
    print(f"{C.GREEN}{msg}{C.RESET}", flush=True)

def log_err(msg: str) -> None:
    print(f"{C.RED}{msg}{C.RESET}", flush=True)

def log_warn(msg: str) -> None:
    print(f"{C.YELLOW}{msg}{C.RESET}", flush=True)

def fail(msg: str) -> None:
    log_err(f"[-] {msg}")

# ---------------------------------------------------------------------------
# GLOBALS
# ---------------------------------------------------------------------------

process = None
module = None
vmm = None
BASE = 0
PROCESS_NAME = "TslGame.exe"
SEARCH_TIMEOUT = 5

# ---------------------------------------------------------------------------
# DMA / MEMORY
# ---------------------------------------------------------------------------

def initialize(process_name: str = "TslGame.exe", device: str = "fpga") -> None:
    global process, module, vmm, BASE, PROCESS_NAME

    PROCESS_NAME = process_name
    vmm = memprocfs.Vmm(["-device", device, "-memmap", "auto", "-disable-symbolserver"])
    process = vmm.process(process_name)

    if not process:
        raise RuntimeError(f"{process_name} not found")

    module = process.module(process_name)
    if not module:
        raise RuntimeError(f"{process_name} module not found")

    BASE = int(module.base)
    log(f"[+] {process_name}  PID={process.pid}  Base=0x{BASE:X}")


def read_mem(addr: int, size: int) -> Optional[bytes]:
    if size <= 0:
        return b""
    try:
        data = process.memory.read(addr, size)
        if not data:
            return None
        return bytes(data)
    except Exception:
        return None


def read_exact(addr: int, size: int) -> Optional[bytes]:
    if size <= 0:
        return b""

    data = read_mem(addr, size)
    if data is not None and len(data) == size:
        return data

    out = bytearray(size)
    got = 0
    page_size = 0x1000

    for page_off in range(0, size, page_size):
        chunk = min(page_size, size - page_off)
        page = read_mem(addr + page_off, chunk)
        if page:
            n = min(len(page), chunk)
            out[page_off:page_off + n] = page[:n]
            got += n

    return bytes(out) if got > 0 else None

# ---------------------------------------------------------------------------
# PE DUMP
# ---------------------------------------------------------------------------

PE_SIGNATURE = b"PE\x00\x00"

def align_up(value: int, alignment: int) -> int:
    if alignment <= 1:
        return value
    return (value + alignment - 1) & ~(alignment - 1)


def dump_pe(out_dir: Path) -> Optional[Path]:
    out_dir.mkdir(parents=True, exist_ok=True)
    out_path = out_dir / f"{PROCESS_NAME.rsplit('.', 1)[0]}_{int(time.time())}.exe"

    header = read_exact(BASE, 0x4000)
    if not header or len(header) < 0x40 or header[:2] != b"MZ":
        fail("Invalid MZ header")
        return None

    e_lfanew = struct.unpack_from("<I", header, 0x3C)[0]
    if e_lfanew + 24 > len(header) or header[e_lfanew:e_lfanew + 4] != PE_SIGNATURE:
        fail("Invalid PE header")
        return None

    coff_off = e_lfanew + 4
    machine, num_sections, _, _, _, opt_size, _ = struct.unpack_from(
        "<HHIIIHH", header, coff_off
    )

    opt_off = coff_off + 20
    if opt_off + opt_size > len(header):
        header = read_exact(BASE, max(0x4000, opt_off + opt_size + num_sections * 40))

    if not header or opt_off + opt_size > len(header):
        fail("Optional header read failed")
        return None

    magic = struct.unpack_from("<H", header, opt_off)[0]
    is64 = magic == 0x20B
    if magic not in (0x10B, 0x20B):
        fail(f"Unsupported PE magic: 0x{magic:X}")
        return None

    image_base_off = opt_off + (24 if is64 else 28)
    section_align_off = opt_off + 32
    file_align_off = opt_off + 36
    size_of_image_off = opt_off + 56
    size_of_headers_off = opt_off + 60
    checksum_off = opt_off + 64
    dll_chars_off = opt_off + 70

    section_alignment = struct.unpack_from("<I", header, section_align_off)[0]
    file_alignment = struct.unpack_from("<I", header, file_align_off)[0]
    size_of_headers = struct.unpack_from("<I", header, size_of_headers_off)[0]
    size_of_image = struct.unpack_from("<I", header, size_of_image_off)[0]

    if not section_alignment:
        section_alignment = 0x1000
    if not file_alignment:
        file_alignment = 0x200

    section_table = opt_off + opt_size
    if section_table + num_sections * 40 > len(header):
        header = read_exact(BASE, section_table + num_sections * 40)
    if not header or section_table + num_sections * 40 > len(header):
        fail("Section table read failed")
        return None

    sections = []
    for index in range(num_sections):
        off = section_table + index * 40
        name = header[off:off + 8].split(b"\x00", 1)[0].decode("ascii", "replace")
        virtual_size = struct.unpack_from("<I", header, off + 8)[0]
        virtual_address = struct.unpack_from("<I", header, off + 12)[0]
        raw_size_old = struct.unpack_from("<I", header, off + 16)[0]
        characteristics = struct.unpack_from("<I", header, off + 36)[0]

        mapped_base = virtual_size if virtual_size else raw_size_old
        mapped_size = align_up(mapped_base, section_alignment)
        if mapped_size == 0:
            continue

        sections.append({
            "index": index,
            "name": name,
            "header_off": off,
            "va": virtual_address,
            "vsize": virtual_size,
            "mapped_size": mapped_size,
            "characteristics": characteristics,
        })

    if not sections:
        fail("No sections found")
        return None

    headers_size = max(size_of_headers, section_table + num_sections * 40)
    file_cursor = align_up(headers_size, file_alignment)

    for sec in sections:
        raw_size = align_up(sec["mapped_size"], file_alignment)
        sec["raw_ptr"] = file_cursor
        sec["raw_size"] = raw_size
        file_cursor += raw_size

    image_end = 0
    for sec in sections:
        image_end = max(image_end, sec["va"] + align_up(sec["mapped_size"], section_alignment))
    image_end = align_up(max(image_end, size_of_image), section_alignment)

    image = bytearray(file_cursor)
    copy_header_size = min(headers_size, len(header))
    image[:copy_header_size] = header[:copy_header_size]

    if is64:
        struct.pack_into("<Q", image, image_base_off, BASE)
    else:
        struct.pack_into("<I", image, image_base_off, BASE & 0xFFFFFFFF)

    struct.pack_into("<I", image, size_of_image_off, image_end)
    struct.pack_into("<I", image, checksum_off, 0)

    dll_chars = struct.unpack_from("<H", image, dll_chars_off)[0]
    dll_chars &= ~0x0040
    struct.pack_into("<H", image, dll_chars_off, dll_chars)

    log(f"[+] PE: {num_sections} sections, ImageSize=0x{image_end:X}, FileSize=0x{len(image):X}")

    for sec in sections:
        h = sec["header_off"]
        raw_ptr = sec["raw_ptr"]
        raw_size = sec["raw_size"]
        va = sec["va"]
        mapped_size = sec["mapped_size"]

        struct.pack_into("<I", image, h + 16, raw_size)
        struct.pack_into("<I", image, h + 20, raw_ptr)

        pos = 0
        while pos < mapped_size:
            chunk = min(0x100000, mapped_size - pos)
            data = read_mem(BASE + va + pos, chunk)

            if data:
                n = min(len(data), chunk)
                image[raw_ptr + pos:raw_ptr + pos + n] = data[:n]

                if n < chunk:
                    tail = read_exact(BASE + va + pos + n, chunk - n)
                    if tail:
                        image[raw_ptr + pos + n:raw_ptr + pos + n + len(tail)] = tail
            else:
                data = read_exact(BASE + va + pos, chunk)
                if data:
                    image[raw_ptr + pos:raw_ptr + pos + len(data)] = data

            pos += chunk

        log(f"    {sec['name']:<8} VA=0x{va:08X} RAW=0x{raw_ptr:08X} SIZE=0x{raw_size:X}")

    if image[:2] != b"MZ" or image[e_lfanew:e_lfanew + 4] != PE_SIGNATURE:
        fail("PE header corrupted after dump")
        return None

    out_path.write_bytes(image)
    sha = hashlib.sha256(image).hexdigest()
    log(f"[+] Dump: {out_path}")
    log(f"[+] SHA256: {sha}")
    return out_path


# ---------------------------------------------------------------------------
# SIGNATURE SCANNER
# ---------------------------------------------------------------------------

def parse_signature(signature: str) -> Tuple[bytes, bytes]:
    pattern = bytearray()
    mask = bytearray()

    for token in signature.split():
        if token in ("?", "??"):
            pattern.append(0)
            mask.append(0xFF)
        else:
            pattern.append(int(token, 16))
            mask.append(0x00)

    return bytes(pattern), bytes(mask)


def get_module_size() -> int:
    try:
        hdr = read_exact(BASE, 0x1000)
        if not hdr or hdr[:2] != b"MZ":
            return 0

        e_lfanew = struct.unpack_from("<I", hdr, 0x3C)[0]
        if e_lfanew + 24 > len(hdr) or hdr[e_lfanew:e_lfanew + 4] != PE_SIGNATURE:
            return 0

        coff_off = e_lfanew + 4
        opt_size = struct.unpack_from("<H", hdr, coff_off + 16)[0]
        opt_off = coff_off + 20
        if opt_off + 60 <= len(hdr):
            return struct.unpack_from("<I", hdr, opt_off + 56)[0]
    except Exception as e:
        log_warn(f"[!] get_module_size error: {e}")
    return 0


def search_signature(signature: str) -> List[int]:
    pattern, mask = parse_signature(signature)
    if len(pattern) > 32:
        log_warn(f"[!] Signature {len(pattern)} bytes, using segmented search...")
        pattern_part = pattern[:32]
        mask_part = mask[:32]
        remaining_pattern = pattern[32:]
        remaining_mask = mask[32:]
    else:
        pattern_part = pattern
        mask_part = mask
        remaining_pattern = None
        remaining_mask = None

    try:
        image_size = get_module_size()
        if image_size <= 0:
            image_size = 0x4000000
            log_warn("[!] Module size unavailable, using fallback 64MB")

        scanner = process.search(
            BASE,
            BASE + image_size,
            memprocfs.FLAG_NOCACHE,
        )
        scanner.add_search(pattern_part, mask_part)
        scanner.start()

        start_time = time.time()
        while not scanner.is_completed:
            time.sleep(0.01)
            if time.time() - start_time > SEARCH_TIMEOUT:
                log_warn(f"[!] Search timeout: {signature[:30]}...")
                scanner.stop()
                return []

        result = scanner.result()
        if not result:
            return []

        if remaining_pattern is not None:
            verified = []
            for addr_tuple in result:
                addr = int(addr_tuple[0])
                data = read_mem(addr + 32, len(remaining_pattern))
                if data is None or len(data) < len(remaining_pattern):
                    continue
                match = True
                for i in range(len(remaining_pattern)):
                    if remaining_mask[i] == 0x00:
                        if data[i] != remaining_pattern[i]:
                            match = False
                            break
                if match:
                    verified.append(addr)
            return verified
        else:
            return [int(x[0]) for x in result]
    except Exception as e:
        log_warn(f"[!] Search error: {e}")
        return []


def scan_one(signature: str) -> Optional[int]:
    matches = search_signature(signature)
    return matches[0] if matches else None


def decode_at(addr: int):
    data = read_mem(addr, 32)
    if not data:
        return None
    return Decoder(64, data, ip=addr).decode()


def get_disp(addr: int) -> Optional[int]:
    insn = decode_at(addr)
    if not insn:
        return None
    return insn.memory_displacement


def get_disp_at_offset(addr: int, offset_index: int = 0) -> Optional[int]:
    if offset_index < 0:
        return None

    data = read_mem(addr, 128)
    if not data:
        return None

    found = 0
    pos = 0
    while pos < len(data) - 5:
        if data[pos] == 0x83 and data[pos+1] == 0xA1:
            if found == offset_index:
                return struct.unpack_from("<I", data, pos + 2)[0]
            found += 1
            pos += 7
        else:
            pos += 1

    return None


def scan_disp(signature: str, skip: int = 0, minimum: int = 0, maximum: int = 0xFFFFFFFF, offset_index: int = 0) -> Optional[int]:
    addr = scan_one(signature)
    if addr is None:
        return None

    if offset_index > 0:
        value = get_disp_at_offset(addr + skip, offset_index)
    else:
        value = get_disp(addr + skip)

    if value is None:
        return None

    if minimum != 0 or maximum != 0xFFFFFFFF:
        if not (minimum <= value <= maximum):
            return None
    return value


def scan_rip(signature: str, skip: int = 0) -> Optional[int]:
    addr = scan_one(signature)
    if addr is None:
        return None
    return get_rip_rva(addr + skip)


def scan_imm(signature: str, skip: int = 0, match_index: int = 0) -> Optional[int]:
    matches = search_signature(signature)
    if not matches:
        return None
    if match_index >= len(matches):
        return None
    addr = matches[match_index]
    return get_imm(addr + skip)


def get_rip_rva(addr: int) -> Optional[int]:
    insn = decode_at(addr)
    if not insn or not insn.is_ip_rel_memory_operand:
        return None
    return (insn.ip_rel_memory_address - BASE) & 0xFFFFFFFF


def get_imm(addr: int) -> Optional[int]:
    insn = decode_at(addr)
    if not insn:
        return None

    for i in range(insn.op_count):
        kind = insn.op_kind(i)
        if kind in (
            OpKind.IMMEDIATE8,
            OpKind.IMMEDIATE16,
            OpKind.IMMEDIATE32,
            OpKind.IMMEDIATE64,
            OpKind.IMMEDIATE32TO64,
            OpKind.IMMEDIATE8TO16,
            OpKind.IMMEDIATE8TO32,
            OpKind.IMMEDIATE8TO64,
        ):
            return insn.immediate(i) & 0xFFFFFFFF
    return None


# ---------------------------------------------------------------------------
# DECRYPT ENGINE
# ---------------------------------------------------------------------------
# Yalnızca 32-bit register ailelerini takip etmek için normalleştirme tablosu.
# Hem 64-bit (RAX, R8 …) hem de 32-bit (EAX, R8D …) varyantlarını aynı
# canonical 32-bit register'a eşler.

_REG32_NORM: Dict = {}
try:
    _reg_pairs = [
        (Register.RAX, Register.EAX),  (Register.EAX, Register.EAX),
        (Register.RBX, Register.EBX),  (Register.EBX, Register.EBX),
        (Register.RCX, Register.ECX),  (Register.ECX, Register.ECX),
        (Register.RDX, Register.EDX),  (Register.EDX, Register.EDX),
        (Register.RSP, Register.ESP),  (Register.ESP, Register.ESP),
        (Register.RBP, Register.EBP),  (Register.EBP, Register.EBP),
        (Register.RSI, Register.ESI),  (Register.ESI, Register.ESI),
        (Register.RDI, Register.EDI),  (Register.EDI, Register.EDI),
        (Register.R8,  Register.R8D),  (Register.R8D,  Register.R8D),
        (Register.R9,  Register.R9D),  (Register.R9D,  Register.R9D),
        (Register.R10, Register.R10D), (Register.R10D, Register.R10D),
        (Register.R11, Register.R11D), (Register.R11D, Register.R11D),
        (Register.R12, Register.R12D), (Register.R12D, Register.R12D),
        (Register.R13, Register.R13D), (Register.R13D, Register.R13D),
        (Register.R14, Register.R14D), (Register.R14D, Register.R14D),
        (Register.R15, Register.R15D), (Register.R15D, Register.R15D),
    ]
    for _r, _r32 in _reg_pairs:
        _REG32_NORM[_r] = _r32
except Exception:
    pass


def _norm32(reg) -> object:
    """Register'ı 32-bit canonical forma döndür (bilinmiyorsa kendisi)."""
    return _REG32_NORM.get(reg, reg)


def _get_imm_any(ins, op_idx: int) -> Optional[int]:
    """Herhangi bir operanddan immediate değerini döndür; immediate değilse None."""
    try:
        k = ins.op_kind(op_idx)
        if k in (
            OpKind.IMMEDIATE8,   OpKind.IMMEDIATE16,  OpKind.IMMEDIATE32,
            OpKind.IMMEDIATE64,  OpKind.IMMEDIATE32TO64,
            OpKind.IMMEDIATE8TO16, OpKind.IMMEDIATE8TO32, OpKind.IMMEDIATE8TO64,
        ):
            return ins.immediate(op_idx) & 0xFFFFFFFF
    except Exception:
        pass
    return None


def extract_cindex_decrypt(match_addr: int, xor_idx: int = 1, method_name: str = "CIndex") -> Optional[Tuple[str, Dict[str, int]]]:
    """
    match_addr adresinden belleği okur ve CIndex decrypt hesaplamasını
    sembolik register takibiyle yeniden oluşturur.

    Her register bir C++ ifade string'i olarak izlenir (örn. "xored", "(xored >> 12)",
    "_rotr(xored, 28)").  Yükleme anından key1 XOR'una kadar kopyalar yayılır;
    key1 XOR'dan sonra SHL/SHR/AND/OR/ROR/XOR/ADD/SUB/NOT/NEG gibi desteklenen
    komutlar sembolik ifadenin üzerine uygulanır.  Sonuç, yığına yapılan ilk
    anlamlı MOV [rsp+N],reg deposunda ya da fallback olarak en uzun ifadeyi
    taşıyan register'da okunur.

    Şablon ifade sabit değildir: her patch'te assembly yapısı değişebileceğinden
    çıktı, disassembly'den dinamik olarak türetilir.
    """
    try:
        # ── Okuma penceresi: match_addr öncesi 64 byte lookback dahil ─────────
        # Bellek yükleme komutu match_addr'den birkaç byte önce olabilir.
        # Lookback bölgesi SADECE val_reg backward scan için kullanılır;
        # xor_hits sadece match_addr sonrasındaki komutlardan seçilir.
        LOOKBACK = 64
        read_start = match_addr - LOOKBACK if match_addr > LOOKBACK else match_addr
        actual_lookback = match_addr - read_start          # gerçek lookback miktarı

        data = read_exact(read_start, 0x100 + actual_lookback)
        if not data:
            data = read_exact(match_addr, 0x100)           # lookback başarısız → normal
            read_start = match_addr; actual_lookback = 0
        if not data:
            log_warn("[!] extract_cindex_decrypt: bellek okunamadı")
            return None

        decoder = Decoder(64, data, ip=read_start)
        instrs: List = []
        while decoder.can_decode and len(instrs) < 120:
            try:
                ins = decoder.decode()
                if ins.is_invalid:
                    break
                instrs.append(ins)
            except Exception:
                break

        if len(instrs) < 4:
            log_warn("[!] extract_cindex_decrypt: yeterli komut decode edilemedi")
            return None

        # ── 1. Büyük sabit XOR'ları tara; xor_idx. eşleşme = key1 ──────────
        # XOR'lar sadece match_addr'den itibaren sayılır (lookback alanındaki
        # yanlış XOR'ların karışmasını önler).
        def _is_big_xor_imm(ins) -> bool:
            if ins.mnemonic != Mnemonic.XOR or ins.op_count != 2:
                return False
            if ins.op_kind(0) != OpKind.REGISTER:
                return False
            imm = _get_imm_any(ins, 1)
            return imm is not None and imm > 0xFFFF

        xor_hits = [(i, ins) for i, ins in enumerate(instrs)
                    if _is_big_xor_imm(ins) and ins.ip >= match_addr]
        if len(xor_hits) <= xor_idx:
            log_warn(
                f"[!] extract_cindex_decrypt: xor_idx={xor_idx} için yeterli XOR-imm bulunamadı "
                f"(bulunan={len(xor_hits)})"
            )
            return None

        key1_idx, key1_ins = xor_hits[xor_idx]
        key1_reg = _norm32(key1_ins.op_register(0))
        key1     = _get_imm_any(key1_ins, 1) & 0xFFFFFFFF

        # ── 2. key1_reg'i besleyen bellek yüklemesini geriye izleyerek bul ────
        # Kopyalama zincirini (MOV dst, src) takip ederek orijinal MOV reg,[mem]
        # komutuna ulaşır.  Bu sayede LODWORD ve HIDWORD ayrı ayrı doğru
        # çıkarılabilir: her blok kendi register'ının mem-load'ını bulur.
        val_reg  = None
        load_idx = None
        current  = key1_reg
        for i in range(key1_idx - 1, -1, -1):
            _ins = instrs[i]
            if (_ins.mnemonic == Mnemonic.MOV and _ins.op_count == 2
                    and _ins.op_kind(0) == OpKind.REGISTER):
                _dst = _norm32(_ins.op_register(0))
                if _dst == current:
                    if _ins.op_kind(1) == OpKind.MEMORY:
                        val_reg  = current
                        load_idx = i
                        break
                    elif _ins.op_kind(1) == OpKind.REGISTER:
                        current = _norm32(_ins.op_register(1))  # kopyayı izle
        if val_reg is None:
            log_warn("[!] extract_cindex_decrypt: key1_reg için bellek yükleme bulunamadı")
            return None

        # ── 3. Ön geçiş: yükleme → key1 XOR aralığında kopya yayılımı ───────
        # Yalnızca MOV reg, reg işlenir; böylece val_reg'in kopyaları izlenir.
        regs: Dict[int, str] = {val_reg: "value"}
        for ins in instrs[load_idx + 1 : key1_idx]:
            if (ins.mnemonic == Mnemonic.MOV and ins.op_count == 2
                    and ins.op_kind(0) == OpKind.REGISTER
                    and ins.op_kind(1) == OpKind.REGISTER):
                src = _norm32(ins.op_register(1))
                dst = _norm32(ins.op_register(0))
                if src in regs:
                    regs[dst] = regs[src]
                elif dst in regs:
                    del regs[dst]

        # ── 4. Key1 XOR → "xored" sembolik değeri ─────────────────────────────
        regs[key1_reg] = "xored"

        # Sabitler sözlüğü — sembolik değerlendirme sırasında doldurulur.
        # (key: constexpr son eki,  value: tamsayı)
        const_map: Dict[str, int] = {"XorKey1": key1}

        # ── 5. Sembolik değerlendirme (elif zinciri — fall-through yok) ──────
        _ALL_IMM_KINDS = (
            OpKind.IMMEDIATE8,     OpKind.IMMEDIATE16,    OpKind.IMMEDIATE32,
            OpKind.IMMEDIATE64,    OpKind.IMMEDIATE32TO64,
            OpKind.IMMEDIATE8TO16, OpKind.IMMEDIATE8TO32, OpKind.IMMEDIATE8TO64,
        )

        def _fmt(expr: str, fname: str = "CIndex") -> str:
            xored_line = f"        DWORD xored = value ^ 0x{key1:08X}U;\n"
            return (
                f"    static inline DWORD {fname}(DWORD value)\n"
                f"    {{\n"
                f"{xored_line}"
                f"        return {expr};\n"
                f"    }}"
            )

        for ins in instrs[key1_idx + 1 :]:
            m = ins.mnemonic
            n = ins.op_count

            # ── Yığına depo: sonuç adayı ──────────────────────────────────
            if (m == Mnemonic.MOV and n == 2
                    and ins.op_kind(0) == OpKind.MEMORY
                    and ins.op_kind(1) == OpKind.REGISTER):
                src = _norm32(ins.op_register(1))
                if src in regs:
                    expr = regs[src]
                    # Minimum uzunluk: "xored" veya "value" tek başına değer değil
                    if ("xored" in expr or "value" in expr) and len(expr) >= 10:
                        return _fmt(expr, method_name), const_map

            # ── MOV reg, reg / MOV reg, [mem] ─────────────────────────────
            elif m == Mnemonic.MOV and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if ins.op_kind(1) == OpKind.REGISTER:
                    src = _norm32(ins.op_register(1))
                    if src in regs:
                        regs[dst] = regs[src]
                    elif dst in regs:
                        del regs[dst]
                else:
                    if dst in regs:
                        del regs[dst]

            # ── XOR reg, imm / XOR reg, reg ───────────────────────────────
            elif m == Mnemonic.XOR and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if ins.op_kind(1) in _ALL_IMM_KINDS:
                    imm = _get_imm_any(ins, 1)
                    if imm is not None and dst in regs:
                        regs[dst] = f"({regs[dst]} ^ 0x{imm:08X}U)"
                        if imm > 0xFFFF and imm != key1 and "XorKey2" not in const_map:
                            const_map["XorKey2"] = imm
                elif ins.op_kind(1) == OpKind.REGISTER:
                    src = _norm32(ins.op_register(1))
                    if dst in regs and src in regs:
                        regs[dst] = f"({regs[dst]} ^ {regs[src]})"
                    elif dst in regs:
                        del regs[dst]

            # ── SHL reg, imm ──────────────────────────────────────────────
            elif m == Mnemonic.SHL and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if dst in regs:
                    sh = _get_imm_any(ins, 1)
                    if sh is not None:
                        regs[dst] = f"({regs[dst]} << {sh & 0xFF})"
                        if "Dval" not in const_map:
                            const_map["Dval"] = sh & 0xFF

            # ── SHR reg, imm ──────────────────────────────────────────────
            elif m == Mnemonic.SHR and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if dst in regs:
                    sh = _get_imm_any(ins, 1)
                    if sh is not None:
                        regs[dst] = f"({regs[dst]} >> {sh & 0xFF})"
                        if "Sval" not in const_map:
                            const_map["Sval"] = sh & 0xFF

            # ── AND reg, imm ──────────────────────────────────────────────
            elif m == Mnemonic.AND and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if dst in regs:
                    imm = _get_imm_any(ins, 1)
                    if imm is not None:
                        regs[dst] = f"({regs[dst]} & 0x{imm:X})"
                        if "XorKey3" not in const_map and imm > 0xFFFF:
                            const_map["XorKey3"] = imm

            # ── OR reg, reg / OR reg, imm ──────────────────────────────────
            elif m == Mnemonic.OR and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if ins.op_kind(1) == OpKind.REGISTER:
                    src = _norm32(ins.op_register(1))
                    if dst in regs and src in regs:
                        regs[dst] = f"({regs[dst]} | {regs[src]})"
                    elif dst in regs:
                        del regs[dst]
                elif ins.op_kind(1) in _ALL_IMM_KINDS:
                    imm = _get_imm_any(ins, 1)
                    if imm is not None and dst in regs:
                        regs[dst] = f"({regs[dst]} | 0x{imm:X})"

            # ── ROR reg, imm ───────────────────────────────────────────────
            elif m == Mnemonic.ROR and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if dst in regs:
                    amt = _get_imm_any(ins, 1)
                    if amt is not None:
                        regs[dst] = f"_rotr({regs[dst]}, {amt & 0xFF})"
                        if "Rval" not in const_map:
                            const_map["Rval"] = amt & 0xFF

            # ── ROL reg, imm ───────────────────────────────────────────────
            elif m == Mnemonic.ROL and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if dst in regs:
                    amt = _get_imm_any(ins, 1)
                    if amt is not None:
                        regs[dst] = f"_rotl({regs[dst]}, {amt & 0xFF})"
                        if "Lval" not in const_map:
                            const_map["Lval"] = amt & 0xFF

            # ── ADD reg, reg / ADD reg, imm ────────────────────────────────
            elif m == Mnemonic.ADD and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if ins.op_kind(1) == OpKind.REGISTER:
                    src = _norm32(ins.op_register(1))
                    if dst in regs and src in regs:
                        regs[dst] = f"({regs[dst]} + {regs[src]})"
                    elif dst in regs:
                        del regs[dst]
                elif ins.op_kind(1) in _ALL_IMM_KINDS:
                    imm = _get_imm_any(ins, 1)
                    if imm is not None and dst in regs:
                        regs[dst] = f"({regs[dst]} + 0x{imm:X})"

            # ── SUB reg, reg / SUB reg, imm ────────────────────────────────
            elif m == Mnemonic.SUB and n == 2 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if ins.op_kind(1) == OpKind.REGISTER:
                    src = _norm32(ins.op_register(1))
                    if dst in regs and src in regs:
                        regs[dst] = f"({regs[dst]} - {regs[src]})"
                    elif dst in regs:
                        del regs[dst]
                elif ins.op_kind(1) in _ALL_IMM_KINDS:
                    imm = _get_imm_any(ins, 1)
                    if imm is not None and dst in regs:
                        regs[dst] = f"({regs[dst]} - 0x{imm:X})"

            # ── NOT reg ────────────────────────────────────────────────────
            elif m == Mnemonic.NOT and n == 1 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if dst in regs:
                    regs[dst] = f"(~{regs[dst]})"

            # ── NEG reg ────────────────────────────────────────────────────
            elif m == Mnemonic.NEG and n == 1 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if dst in regs:
                    regs[dst] = f"(-(int){regs[dst]})"

            # ── CMP / TEST: salt okuma, izlenen register'ı bozmaz ─────────
            elif m in (Mnemonic.CMP, Mnemonic.TEST):
                pass

            # ── Diğer komutlar: hedef register'ı geçersiz kıl ────────────
            elif n >= 1 and ins.op_kind(0) == OpKind.REGISTER:
                dst = _norm32(ins.op_register(0))
                if dst in regs:
                    del regs[dst]

        # ── Fallback: en uzun anlamlı ifadeyi taşıyan register ───────────────
        best_expr: Optional[str] = None
        best_len  = 0
        for expr in regs.values():
            if ("xored" in expr or "value" in expr) and len(expr) > best_len:
                best_expr = expr
                best_len  = len(expr)
        if best_expr and best_len >= 10:
            return _fmt(best_expr, method_name), const_map

        log_warn("[!] extract_cindex_decrypt: sonuç ifadesi bulunamadı")
        return None

    except Exception as e:
        log_warn(f"[!] extract_cindex_decrypt genel hata: {e}")
        return None


def scan_decrypt_method(
        signature: str,
        match_index: int,
        xor_idx: int = 1,
        method_name: str = "CIndex",
        _sig_cache: Optional[Dict] = None,
) -> Optional[Tuple[str, Dict[str, int]]]:
    """
    Verilen imzanın match_index. eşleşmesini bulur ve
    o adresteki decrypt metodunu + sabitlerini dinamik olarak çıkarır.

    match_index = -1  →  OTOMATİK: tüm eşleşmeler sırayla denenir,
                          ilk başarılı çıkarım döndürülür.
    _sig_cache       →  İsteğe bağlı önbellek; aynı imzayı tekrar aramayı önler.
    Dönüş: (method_body_str, const_map) ya da None.
    """
    try:
        if _sig_cache is not None and signature in _sig_cache:
            matches = _sig_cache[signature]
        else:
            matches = search_signature(signature)
            if _sig_cache is not None:
                _sig_cache[signature] = (matches or [])

        if not matches:
            return None

        if match_index == -1:
            indices: range = range(len(matches))      # OTOMATİK: hepsini dene
        elif match_index < len(matches):
            indices = range(match_index, match_index + 1)
        else:
            return None

        for i in indices:
            addr = matches[i]
            result = extract_cindex_decrypt(addr, xor_idx, method_name)
            if result is not None:
                log_info(
                    f"[*] Decrypt imzası 0x{addr:X} adresinde "
                    f"(eşleşme #{i + 1}/{len(matches)})"
                )
                return result
        return None

    except Exception as e:
        log_warn(f"[!] scan_decrypt_method genel hata: {e}")
        return None


# ---------------------------------------------------------------------------
# OFFSETS
# ---------------------------------------------------------------------------

SIGNATURES = [
    ("GNames", "rip",  "48 8D 0D ? ? ? ? 48 83 3D ? ? ? ? ? 75 ? 48 8B D1 B9 ? ? ? ? 48 8B 05", 0, 0, 0, 0),
    ("GObjects", "rip", "3B 35 ? ? ? ? 0F 8D ? ? ? ? 48 8B 15", 0, 0, 0, 0),
    ("GObjectsCount", "rip", "44 3B 35 ? ? ? ? 0F 8D ? ? ? ? 48 8B 15", 13, 0, 0, 0),
    ("XenuineDecrypt", "rip", "48 8B 05 ? ? ? ? FF D0 48 8B D0", 0, 0, 0, 0),
    ("ChunkSize", "imm", "69 C7 ? ? ? ? 44 2B F0", 0, 0, 0, 0),
    ("PhysxSDK", "rip", "48 8B 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 5C 24 ? 55 56 57 48 83 EC", 0, 0, 0, 0),
    ("UWorld", "rip", "48 89 05 ? ? ? ? 75 ? 48 8B D0 B9", 0, 0, 0, 0),
    ("GameInstance", "disp", "48 8B 88 ? ? ? ? 75 ? 48 8B 05 ? ? ? ? 48 8B D1 B9 ? ? ? ? FF D0 EB ? 8D 81 ? ? ? ? 48 C1 E9 ? 35 ? ? ? ? 89 44 24 ? B8 ? ? ? ? 2B C8 33 C8 89 4C 24 ? 48 8B 44 24 ? 48 8B C8", 0, 0, 0xFFFF, 0),
    ("GameState", "disp", "48 8B 8B 78 02 00 00", 0, 0x0, 0x600, 0),
    ("CurrentLevel", "disp", "48 8B 8E ? ? ? ? 75 14 4C 8B 05", 0, 0x100, 0x400, 0),
    ("LocalPlayer", "disp", "48 89 5C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 33 DB 48 8B F2 48 85 D2", 0x25, 0x30, 0x200, 0),
    ("Actors", "disp", "48 8B 88 ? ? ? ? 48 39 1D ? ? ? ? 75 13 48 8B D1", 0, 0x30, 0x300, 0),
    ("TimeSeconds", "disp", "F3 0F 10 83 ? ? ? ? F3 0F 10 8B ? ? ? ? F3 0F 58 C1", 0, 0x20, 0xFFFFFFFF, 0),
    ("WorldToMap", "disp", "44 8B 83 ? ? ? ? 44 89 44 24", 0, 0, 0xFFFFFFFF, 0),
    ("PlayerController", "disp", "49 8B 4E 38 0F", 0, 0x30, 0x50, 0),
    ("AcknowledgedPawn", "disp", "48 8B 87 ? ? ? ? 48 89 44 24 ? EB ? 48 8B 44 24 ? 48 85 C0 0F 84", 0, 0x100, 0x600, 0),
    ("PlayerCameraManager", "disp", "48 8B 8B ? ? ? ? 48 85 C9 74 ? F3 0F 10 81", 0, 0x400, 0x600, 0),
    ("RootComponent", "disp", "48 8B 8B ? ? ? ? 75 13 48 8B 05 ? ? ? ? 48 8B D1 B9 ? ? ? ? FF D0 EB 31", 0, 0x100, 0x400, 0),
    ("ViewTarget", "disp", "48 8B 8B 80 16 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("MyHUD", "disp", "48 8B 8B D0 04 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("BlockInputWidgetList", "disp", "48 8B 87 ? ? ? ? FF 08", 0, 0, 0x1400, 0),
    ("WidgetStateMap", "disp", "48 8D 87 ? ? ? ? 48 8D B3 ? ? ? ? 48 3B F0 74 ? 48 63 68 ? 4C 8B 30 44 8B 46 ? 89 6E ? 85 ED 0F 85 ? ? ? ? 45 85 C0 0F 85 ? ? ? ? 44 89 7E ? 48 8D 87 ? ? ? ? 48 8D B3", 0, 0x400, 0x1400, 0),
    ("SelectMinimapSizeIndex", "disp", "8B 8B ? ? ? ? 89 4C 24 ? 48 8B 83 ? ? ? ? 48 8B", 0, 0x400, 0x1400, 0),
    ("TrainingMapGrid", "disp", "48 8B 8F ? ? ? ? E8 ? ? ? ? 8B 97 ? ? ? ? 48 8B CF", 0, 0, 0x800, 0),
    ("Mesh", "disp", "48 8B 99 ? ? ? ? 48 85 DB 74 ? F6 83", 0, 0x400, 0x800, 0),
    ("LastTeamNum", "disp", "F7 ? ? ? ? ? ? 75 ? 48 8B ? 48 8B ? FF 92 ? ? ? ? 41 3B 86", 21, 0x1000, 0x2000, 0),
    ("CharacterName", "disp", "48 8D 97 ? ? ? ? 48 8D 8C 24 ? ? ? ? E8 ? ? ? ? 90 4C 89 64 24", 0, 0x1000, 0x2200, 0),
    ("SpectatedCount", "disp", "83 B8 ? ? ? ? ? 0F 8F ? ? ? ? 0F 28 74 24", 0, 0, 0xFFFFFFFF, 0),
    ("ReplicatedMovement", "disp", "44 0F 10 96 ? ? ? ? 8B 86", 0, 0x40, 0x200, 0),
    ("Gender", "disp", "41 8A 87 ? ? ? ? 41 38 86", 0, 0, 0x1200, 0),
    ("CharacterMovement", "disp", "48 8B 8F 20 07 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("LastUpdateVelocity", "disp", "F2 0F 10 80 ? ? ? ? F2 0F 11 44 24 ? 0F 11 4C 24 ? E8 ? ? ? ? 48 8B D0 49 8B CE", 0, 0, 0x800, 0),
    ("CharacterState", "disp", "0F B6 83 ? ? ? ? 3C ? 77 ? 48 8B 5C 24", 0, 0x1600, 0x1700, 0),
    ("InventoryFacade", "disp", "48 8B 83 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 8B 40 ? 8B F0 81 F6 ? ? ? ? C1 E0 ? C1 CE ? 25 ? ? ? ? 33 F0 81 F6 ? ? ? ? 3B 35 ? ? ? ? 7D ? 48 8B 15 ? ? ? ? 4D 85 C0 75 ? 48 8B 05 ? ? ? ? B9 ? ? ? ? FF D0 48 8B D0 EB ? 8B C2 48 C1 EA ? 35 ? ? ? ? 81 F2 ? ? ? ? 2D ? ? ? ? 81 EA ? ? ? ? 35 ? ? ? ? 81 F2 ? ? ? ? 89 44 24 ? 89 54 24 ? 48 8B 54 24 ? 48 63 C6 48 8D 0C 40 48 8D 3C CA F7 47 ? ? ? ? ? 74 ? 48 8B CB", 0, 0, 0xFFFFFFFF, 0),
    ("AntiCheatCharacterSyncManager", "disp", "48 8B 83 ? ? ? ? 48 85 C0 74 ? 8B 40 ? 44 8B C0", 0, 0, 0xFFFFFFFF, 0),
    ("FeatureRepObject", "disp", "48 8B 83 ? ? ? ? 4C 8D 0D ? ? ? ? 4C 8B C7 48 8D 55 ? 48 8B 88 ? ? ? ? FF 15 ? ? ? ? 48 89 83 ? ? ? ? 4C 39 BB ? ? ? ? 0F 85 ? ? ? ? 33 D2 49 8B CC E8 ? ? ? ? 48 8B F8 48 89 45 ? 48 85 C0 74 ? 4D 8B C4 33 D2 48 8B C8 E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 07 4C 89 7F ? 44 89 7F ? 48 8D 05 ? ? ? ? 48 89 47 ? EB ? 49 8B FF 48 89 BB ? ? ? ? 8B 45 ? 89 45 ? 48 89 5D ? 48 8D 55 ? 48 8D 4D ? E8 ? ? ? ? 48 8D 4F ? 48 8B D0 E8 ? ? ? ? 44 89 6D ? 48 8B 83 ? ? ? ? 4C 8D 0D ? ? ? ? 4C 8B C7 48 8D 55 ? 48 8B 88 ? ? ? ? FF 15 ? ? ? ? 48 89 83 ? ? ? ? 4C 39 BB", 0, 0, 0xFFFFFFFF, 0),
    ("ComponentToWorld", "disp", "0F 10 80 ? ? ? ? 0F 11 43 20 0F 10 88", 0, 0x200, 0x500, 0),
    ("ComponentLocation", "disp", "0F 10 80 ? ? ? ? 0F 11 43 20 0F 10 88", 11, 0x200, 0x500, 0),
    ("ComponentVelocity", "disp", "F2 0F 11 82 ? ? ? ? 8B 81 ? ? ? ? 89 82", 0, 0x100, 0x400, 0),
    ("StaticMesh", "disp", "48 03 94 CE", 0, 0, 0xFFFFFFFF, 0),
    ("AnimScriptInstance", "disp", "48 8B 81 ? ? ? ? 48 85 C0 74 ? 80 78 ? ? 75 ? 48 8B 88", 0, 0xC00, 0x1200, 0),
    ("bAlwaysCreatePhysicsState", "disp", "83 8F ? ? ? ? ? 48 8B 5C 24 ? 48 8B 74 24 ? 48 83 C4 20 5F C3", 0, 0x300, 0x600, 0),
    ("Eyes", "disp", "0F 2F 83 ? ? ? ? 41 0F 43 CE 83 E0 FB 0B C8", 0, 0x600, 0xC00, 0),
    ("LastSubmitTime", "disp", "F3 0F 10 9B ? ? ? ? 0F 28 CA F3 0F 5C CB F3 0F 10 05 ? ? ? ? 0F 2F C1", 0, 0x600, 0xC00, 0),
    ("CameraCacheFOV", "disp", "F3 0F 10 81 ? ? ? ? 0F 2F 05 ? ? ? ? 77 08 F3 0F 10 81", 18, 0x200, 0x1800, 0),
    ("CameraCacheLocation", "disp", "F2 0F 10 81 ? ? ? ? F2 0F 11 07 8B 81 ? ? ? ? 89 47 08 F2 0F 10 81", 0, 0x200, 0x1800, 0),
    ("CameraCacheRotation", "disp", "F2 0F 10 81 ? ? ? ? F2 0F 11 06 8B 81 ? ? ? ?", 0, 0x200, 0x1800, 0),
    ("WeaponProcessor", "disp", "48 8B 8B ? ? ? ? E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 48 8B CE", 0, 0x500, 0x1200, 0),
    ("EquippedWeapons", "disp", "48 8B 91 ? ? ? ? 48 63 80 ? ? ? ? 48 8B CA 4C 8D 0C C2 49 3B D1", 0, 0x100, 0x400, 0),
    ("CurrentWeaponIndex", "disp", "44 0F B6 C2 84 D2 74 0E 41 83 F8 01 75 10 0F BE 81 ? ? ? ?", 14, 0x200, 0x500, 0),
    ("WeaponTrajectoryData", "disp", "48 8B ? ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3", 0, 0x900, 0x1300, 0),
    ("TrajectoryConfig", "disp", "F7 43 ? ? ? ? ? 75 ? 48 8B ? E8 ? ? ? ? F3 0F 10 80 ? ? ? ? EB", 17, 0x80, 0x200, 0),
    ("BallisticCurve", "disp", "48 8B 80 ? ? ? ? 48 85 C0 74 ? 48 8B 40", 0, 0, 0x70, 0),
    ("TrajectoryGravityZ", "disp", "F3 0F 10 87 EC 10 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("FiringAttachPoint", "disp", "4C 8D 8B ? ? ? ?", 0, 0, 0x900, 0),
    ("ScopingAttachPoint", "disp", "48 8D BB 28 0B 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("WeaponConfig_WeaponClass", "disp", "0F B6 83 ? ? ? ? 3C ? 77 ? 0F B6 C0", 0, 0x794, 0x7A0, 0),
    ("CurrentAmmoData", "disp", "66 8B 83 ? ? ? ? 66 89 44 24", 0, 0xB30, 0xB50, 0),
    ("AttachedStaticComponentMap", "disp", "48 8D 8F ? ? ? ? 48 8B D3 E8 ? ? ? ? 48 8B 97", 0, 0x1504, 0x1510, 0),
    ("AttachedItems", "disp", "48 8D 97 ? ? ? ? 48 8B CB E8", 0, 0x854, 0x88C, 0),
    ("WeaponAttachmentData", "disp", "48 8B 83 ? ? ? ? 48 85 C0 74 ? 8B 40 ? 89 87", 0, 0x124, 0x130, 0),
    ("ElapsedCookingTime", "disp", "F3 0F 10 83 ? ? ? ? F3 0F 59 05 ? ? ? ? F3 0F 5C C8", 0, 0xB4C, 0xC30, 0),
    ("Mesh3P", "disp", "48 8B 83 ? ? ? ? 48 85 C0 74 ? 48 8B 80 ? ? ? ? 48 85 C0 74 ? 8B 40", 0, 0x7FC, 0x830, 0),
    ("StaticSockets", "disp", "48 8D 83 ? ? ? ? 48 89 44 24 ? 8B 83 ? ? ? ? 85 C0 0F 84", 0, 0xC4, 0xD0, 0),
    ("ControlRotation_CP", "disp", "F2 0F ? ? ? ? ? ? 44 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 44 ? ? ? ? ? ? 44 ? ? ? ? ? ? 44 ? ? ? ? ? ? ? 44 ? ? ? ? ? ? ? ? ? 44", 0, 0x400, 0x900, 0),
    ("RecoilADSRotation_CP", "disp", "F3 0F 10 8F ? ? ? ? 48 8B 8F ? ? ? ? F3 0F 10", 0, 0x700, 0xC00, 0),
    ("LeanLeftAlpha_CP", "disp", "F3 44 0F 11 ? ? ? ? ? 84 C9", 0, 0x500, 0x900, 0),
    ("LeanRightAlpha_CP", "disp", "F3 44 0F 11 93 ? ? ? ? 44 0F 28 C7", 0, 0x500, 0x900, 0),
    ("bIsScoping_CP", "disp", "F3 0F 11 83 ? ? ? ? 80 BB ? ? ? ? 00 74", 8, 0x850, 0x870, 0),
    ("bIsReloading_CP", "disp", "44 38 A3 ? ? ? ? 75 ? 44 38 A3", 0, 0x730, 0x748, 0),
    ("bIsDBNO_CP", "disp", "8A ? 31 09 00 00", 0, 0x92F, 0x935, 0),
    ("PreEvalPawnState", "disp", "0F B6 ? 38 06 00 00", 0, 0x636, 0x63C, 0),
    ("LeanActiveFlag_CP", "disp", "8A 43 ? F3 0F 10 83 9C 06 00 00", 0, 0x696, 0x69C, 0),
    ("LeanRightFlag_CP", "disp", "8A 4B ? F3 0F 10 83 A0 06 00 00", 0, 0x698, 0x69E, 0),
    ("MouseX", "disp", "0F B6 81 ? ? ? ? ? ? ? ? ? ? ? ? ? 0F B6 89", 0, 0x4C00, 0x4D00, 0),
    ("InputAxisProperties", "disp", "48 8D 83 ? ? ? ? 48 8B CE E8 ? ? ? ? 8B 83 ? ? ? ?", 0, 0x100, 0x490, 0),
    ("AimOffsets", "disp", "F3 0F 10 86 20 1A 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("VehicleRiderComponent", "disp", "48 8B 87 ? ? ? ? 8B 80 ? ? ? ? C1 E8 1F 34 01", 0, 0x1800, 0x3000, 0),
    ("SeatIndex", "disp", "48 8B 87 ? ? ? ? 8B 80 ? ? ? ? C1 E8 1F 34 01", 7, 0x100, 0x400, 0),
    ("LastVehiclePawn", "disp", "8B ? ? ? ? ? 0F 57 F6 48 85 ? 0F 84 ? ? ? ? 8B", 0, 0x220, 0x300, 0),
    ("VehicleMovement", "disp", "48 8B 87 ? ? ? ? 48 85 C0 74 ? E8 ? ? ? ? 84 C0 74 ? 48 8B 8F", 0, 0x464, 0x4A0, 0),
    ("VehicleCommonComponent", "disp", "48 8B 8F 30 0B 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("VehicleHealth", "disp", "F3 0F 10 87 D8 02 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("VehicleFuel", "disp", "F3 0F 10 87 ? ? ? ? F3 0F 11 87 ? ? ? ? F3 0F 10 87", 0, 0x2DE, 0x2E4, 0),
    ("Wheels", "disp", "48 8D 83 ? ? ? ? 48 89 44 24 ? 8B 83 ? ? ? ? 85 C0", 0, 0x324, 0x330, 0),
    ("DampingRate", "disp", "F3 0F 10 81 B4 00 00 00", 4, 0x40, 0xC0, 0),
    ("ShapeRadius", "disp", "F3 0F 10 81 4C 00 00 00", 4, 0x40, 0x60, 0),
    ("PlayerArray", "disp", "48 8D 8F 10 04 00 00", 3, 0x400, 0x430, 0),
    ("NumAliveTeams", "disp", "89 83 80 04 00 00", 2, 0x470, 0x4C0, 0),
    ("HeaFlag", "disp", "80 B9 ? ? ? ? ? 74 ? 83 B9 ? ? ? ? ? 74 ? 80 B9", 0x00, 0, 0xFFFFFFFF, 0),
    ("Health1", "disp", "80 B9 ? ? ? ? ? 74 ? 83 B9 ? ? ? ? ? 74 ? 80 B9", 0x09, 0, 0xFFFFFFFF, 0),
    ("Health2", "disp", "80 B9 ? ? ? ? ? 74 ? 83 B9 ? ? ? ? ? 74 ? 80 B9", 0x4A, 0, 0xFFFFFFFF, 0),
    ("Health3", "disp", "80 B9 ? ? ? ? ? 74 ? 83 B9 ? ? ? ? ? 74 ? 80 B9", 0x19, 0, 0xFFFFFFFF, 0),
    ("Health4", "disp", "80 B9 ? ? ? ? ? 74 ? 83 B9 ? ? ? ? ? 74 ? 80 B9", 0x20, 0, 0xFFFFFFFF, 0),
    ("Health5", "disp", "80 B9 ? ? ? ? ? 74 ? 83 B9 ? ? ? ? ? 74 ? 80 B9", 0x12, 0, 0xFFFFFFFF, 0),
    ("Health6", "disp", "80 B9 ? ? ? ? ? 74 ? 83 B9 ? ? ? ? ? 74 ? 80 B9", 0x31, 0, 0xFFFFFFFF, 0),
    ("Health_keys0", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 4, 0, 0xFFFFFFFF,0),
    ("Health_keys1", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 13, 0, 0xFFFFFFFF,0),
    ("Health_keys2", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 20, 0, 0xFFFFFFFF,0),
    ("Health_keys3", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 27, 0, 0xFFFFFFFF,0),
    ("Health_keys4", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 34, 0, 0xFFFFFFFF,0),
    ("Health_keys5", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 41, 0, 0xFFFFFFFF,0),
    ("Health_keys6", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 48, 0, 0xFFFFFFFF,0),
    ("Health_keys7", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 55, 0, 0xFFFFFFFF,0),
    ("Health_keys8", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 62, 0, 0xFFFFFFFF,0),
    ("Health_keys9", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 69, 0, 0xFFFFFFFF,0),
    ("Health_keys10", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 76, 0, 0xFFFFFFFF,0),
    ("Health_keys11", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 83, 0, 0xFFFFFFFF,0),
    ("Health_keys12", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 90, 0, 0xFFFFFFFF,0),
    ("Health_keys13", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 97, 0, 0xFFFFFFFF,0),
    ("Health_keys14", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 104, 0, 0xFFFFFFFF,0),
    ("Health_keys15", "imm", "48 89 45 F0 C7 45 ? ? ? ? ? 33 D2 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45", 111, 0, 0xFFFFFFFF,0),
    ("GroggyHealth", "disp", "83 A1 ? ? ? ? ? 83 A1 ? ? ? ? ? 83 A1 ? ? ? ? ? 83 A1 ? ? ? ? ? 83 A1 ? ? ? ? ? 0F 57 F6", 0, 0x1000, 0x2000, 0),
    ("PlayerState", "disp", "48 8B 8F ? ? ? ? 48 85 C9 74 ? 48 8B ? 48 8B", 0, 0x414, 0x460, 0),
    ("PlayerName", "disp", "48 8D 96 20 04 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("PlayerStatusType", "disp", "0F B6 83 ? ? ? ? 83 F8 ? 74 ? 83 F8 ? 74", 0, 0x464, 0x4A0, 0),
    ("SquadMemberIndex", "disp", "8B 83 ? ? ? ? 89 44 24 ? 33 D2", 0, 0xA18, 0xAB0, 0),
    ("AccountId", "disp", "48 8D 96 ? ? ? ? 48 8D 8C 24 ? ? ? ? E8 ? ? ? ? 90", 0, 0x800, 0x960, 0),
    ("DamageDealtOnEnemy", "disp", "F3 0F 58 83 04 08 00 00", 0, 0x500, 0x810, 0),
    ("TeamNumber", "disp", "8B 83 C4 07 00 00", 2, 0x690, 0x7D0, 0),
    ("PartnerLevel", "disp", "0F B6 83 C6 06 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("SurvivalTier", "disp", "8B 83 ? ? ? ? 89 44 24 ? 8B 83 ? ? ? ?", 0, 0xCC6, 0xCCE, 0),
    ("CharacterClanInfo", "disp", "48 8D 83 ? ? ? ? 48 8D 8C 24 ? ? ? ? E8", 0, 0x836, 0x840, 0),
    ("PlayerStatistics", "disp", "8B 83 ? ? ? ? FF C0 89 83 ? ? ? ?", 0, 0x7F8, 0x808, 0),
    ("SafetyZonePosition", "disp", "F3 0F 10 87 ? ? ? ? F3 0F 10 8F ? ? ? ?", 0, 0xAE, 0xB8, 0),
    ("SafetyZoneRadius", "disp", "F3 0F 10 87 BC 00 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("BlueZoneRadius", "disp", "F3 0F 10 87 ? ? ? ? F3 0F 58 87 ? ? ? ? F3 0F 11", 0, 0xCA, 0xD0, 0),
    ("Inventory", "disp", "48 8B 87 ? ? ? ? 48 8B 8F ? ? ? ? 48 85 C0", 0, 0x3FE, 0x406, 0),
    ("InventoryItems", "disp", "48 8D 83 90 06 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("ItemID", "disp", "48 8B 87 ? ? ? ? 48 85 C0 74 ? 8B 40 08", 14, 0, 0x260, 0),
    ("ItemsArray", "disp", "48 8D 83 60 05 00 00", 0, 0, 0xFFFFFFFF, 0),
    ("Durability", "disp", "F3 0F 10 83 ? ? ? ? F3 0F 10 8B ? ? ? ? F3 0F 58 C1", 0, 0x1E2, 0x1EA, 0),
    ("DroppedItem", "disp", "48 8B 87 ? ? ? ? 48 85 C0 74 ? 48 8B 50 ? 48 85 D2 74", 0, 0x41C, 0x480, 0),
    ("DroppedItemGroup", "disp", "48 8B 87 C0 01 00 00", 3, 0, 0xFFFFFFFF, 0),
    ("ItemPackageItems", "disp", "48 8D 83 78 05 00 00", 3, 0, 0xFFFFFFFF, 0),
    ("DroppedItemGroupUItem", "disp", "48 8B 83 70 08 00 00", 3, 0, 0xFFFFFFFF, 0),
    ("TimeTillExplosion", "disp", "F3 0F 10 83 ? ? ? ? F3 0F 5C 83 ? ? ? ? F3 0F 11 83", 0, 0x822, 0x82A, 0),
    ("ExplodeState", "disp", "0F B6 83 ? ? ? ? 83 F8 ? 74 ? 83 F8 ? 74 ? 83 F8 ?", 0, 0x624, 0x660, 0),
    ("MortarRotation", "disp", "48 8D 83 ? ? ? ? 33 C9 E8", 0, 0x514, 0x550, 0),

]

# ---------------------------------------------------------------------------
# DECRYPT METHOD SIGNATURES
# ---------------------------------------------------------------------------
# Her giriş: (metod_adı, imza, match_index)
#   metod_adı   – offsets.hpp içindeki "struct Decrypt" içindeki C++ metod adı
#   imza        – taranacak byte deseni (? = wildcard)
#   match_index – hangi eşleşmenin kullanılacağı (0-bazlı); 1 → 2. eşleşme
#
# Bu imza 4 sonuç döndürür; 2. sonuç (index=1) CIndex şifre çözme bloğunu
# barındıran fonksiyona aittir:
#   .text:00007FF62CF78B28  sub_7FF62CF78908  mov r9, rax
#
# (method_name, signature, match_index, const_prefix, xor_idx)
#
# match_index = -1  →  OTOMATİK MOD: imzanın tüm eşleşmeleri sırayla denenir,
#   ilk başarılı çıkarım kullanılır. Her yamada match_index güncellemeye gerek yok.
#
# xor_idx: fonksiyon içindeki kaçıncı büyük XOR-imm'nin key1 olacağı
#   0 → 1. büyük XOR (Lo/ilk blok),  1 → 2. büyük XOR (Hi/ikinci blok)
#
# Aynı method_name için birden fazla imza varyantı eklenebilir. İlk başarılı
# olanın bulunmasıyla diğer varyantlar atlanır.  Yeni bir patch geldiğinde
# SADECE yeni imzayı buraya eklemek yeterlidir.
DECRYPT_SIGNATURES: List[Tuple[str, str, int, str, int]] = [
    # ── Lo bloğu (xor_idx=0) ────────────────────────────────────────────────
    ("CIndexLo", "8B ? ? 8B ? ? 81 F2 ? ? ? ? 8B ? 8B", -1, "DecryptNameIndexLo", 0),
    ("CIndexLo", "8B ? ? 8B ? ? 81 F2",                  -1, "DecryptNameIndexLo", 0),
    ("CIndexLo", "8B ? ? 8B ? ? 81 F0 ? ? ? ? 8B ? 8B", -1, "DecryptNameIndexLo", 0),
    ("CIndexLo", "8B ? ? 8B ? ? 81 F0",                  -1, "DecryptNameIndexLo", 0),
    ("CIndexLo", "8B ? ? 8B ? ? 81 F1 ? ? ? ? 8B ? 8B", -1, "DecryptNameIndexLo", 0),
    ("CIndexLo", "8B ? ? 8B ? ? 81 F1",                  -1, "DecryptNameIndexLo", 0),
    ("CIndexLo", "8B ? ? 8B ? ? 81 F3",                  -1, "DecryptNameIndexLo", 0),
    # ── Hi bloğu (xor_idx=1) ────────────────────────────────────────────────
    ("CIndexHi", "8B ? ? 8B ? ? 81 F2 ? ? ? ? 8B ? 8B", -1, "DecryptNameIndexHi", 1),
    ("CIndexHi", "8B ? ? 8B ? ? 81 F2",                  -1, "DecryptNameIndexHi", 1),
    ("CIndexHi", "8B ? ? 8B ? ? 81 F0 ? ? ? ? 8B ? 8B", -1, "DecryptNameIndexHi", 1),
    ("CIndexHi", "8B ? ? 8B ? ? 81 F0",                  -1, "DecryptNameIndexHi", 1),
    ("CIndexHi", "8B ? ? 8B ? ? 81 F1 ? ? ? ? 8B ? 8B", -1, "DecryptNameIndexHi", 1),
    ("CIndexHi", "8B ? ? 8B ? ? 81 F1",                  -1, "DecryptNameIndexHi", 1),
    ("CIndexHi", "8B ? ? 8B ? ? 81 F3",                  -1, "DecryptNameIndexHi", 1),

    # ── EAX kısa form (35 opcode: XOR EAX, imm32) ───────────────────────────
    # Bazı yamalarda XOR EAX, key1 kısa form (opcode 35) kullanılır.
    # Pattern: MOV reg,[mem]  → XOR EAX,imm32 (35)  → MOV reg,reg  → C1(ROR/SHL)
    ("CIndexLo", "8B ? ? 35 ? ? ? ? 8B ? ? C1 ? ? C1 ? ? 25 ? ? ? ? 33", -1, "DecryptNameIndexLo", 0),
    ("CIndexLo", "8B ? ? 35 ? ? ? ? 8B ? ? C1",                           -1, "DecryptNameIndexLo", 0),
    ("CIndexHi", "8B ? ? 35 ? ? ? ? 8B ? ? C1 ? ? C1 ? ? 25 ? ? ? ? 33", -1, "DecryptNameIndexHi", 1),
    ("CIndexHi", "8B ? ? 35 ? ? ? ? 8B ? ? C1",                           -1, "DecryptNameIndexHi", 1),
]


def scan_all_decrypt_methods() -> Tuple[Dict[str, str], Dict[str, int]]:
    """
    Tüm decrypt imzalarını tara ve metodları + sabitleri çıkar.
    Dönüş: (method_dict, const_dict)
      method_dict: {method_name: body_str}
      const_dict : {constexpr_name: int_value}  (prefix + suffix dahil)
    """
    method_dict: Dict[str, str] = {}
    const_dict:  Dict[str, int] = {}

    if not DECRYPT_SIGNATURES:
        return method_dict, const_dict

    log_info("╭────────────────────────────────────────────────────────────╮")
    log_info("│  DECRYPT METHOD SCAN                                       │")
    log_info("├────────────────────────────────────────────────────────────┤")

    sig_cache: Dict[str, list] = {}   # aynı imzayı tekrar aramayı önler
    seen_miss: set = set()             # MISS loglamasını tekrarlamayı önler

    for name, sig, idx, prefix, xor_idx in DECRYPT_SIGNATURES:
        if name in method_dict:
            continue                  # bu metod zaten başarıyla bulundu
        try:
            result = scan_decrypt_method(sig, idx, xor_idx, name, sig_cache)
            if result:
                body, cmap = result
                method_dict[name] = body
                for suffix, val in cmap.items():
                    const_dict[f"{prefix}{suffix}"] = val
                log_ok(f"[+] Decrypt::{name:<26} çıkarıldı")
            # MISS — sessiz; bir sonraki varyant denenecek
        except Exception as e:
            log_warn(f"[!] Decrypt::{name} beklenmeyen hata: {e}")

    # Hiçbir varyant bulamadıysa tek sefer MISS logla
    for name, *_ in DECRYPT_SIGNATURES:
        if name not in method_dict and name not in seen_miss:
            log_err(f"[-] Decrypt::{name:<26} MISS")
            seen_miss.add(name)

    total = len(DECRYPT_SIGNATURES)
    found = len(method_dict)
    pad   = 43 - len(str(found)) - len(str(total))
    log_info("├────────────────────────────────────────────────────────────┤")
    log_info(f"│  Çıkarılan: {C.BOLD}{found}/{total}{C.RESET}{C.CYAN} decrypt metod{' ' * pad}│")
    log_info("╰────────────────────────────────────────────────────────────╯")

    return method_dict, const_dict


def scan_all_offsets() -> Dict[str, int]:
    result: Dict[str, int] = {}

    log_info("╭────────────────────────────────────────────────────────────╮")
    log_info("│  OFFSET SCAN                                               │")
    log_info("├────────────────────────────────────────────────────────────┤")

    for name, kind, signature, skip, minimum, maximum, offset_index in SIGNATURES:
        value = None
        try:
            if kind == "disp":
                value = scan_disp(signature, skip, minimum, maximum, offset_index)
            elif kind == "rip":
                value = scan_rip(signature, skip)
            elif kind == "imm":
                value = scan_imm(signature, skip, offset_index)
            else:
                log_warn(f"[!] Unknown kind: {kind}")
        except Exception as _scan_err:
            log_err(f"[-] {name:<30} HATA: {_scan_err}")
            value = None

        if value is not None:
            result[name] = value
            log_ok(f"[+] {name:<30} = 0x{value:X}")
        else:
            log_err(f"[-] {name:<30} MISS")

    total = len(SIGNATURES)
    found = len(result)
    log_info("├────────────────────────────────────────────────────────────┤")
    log_info(f"│  Resolved: {C.BOLD}{found}/{total}{C.RESET}{C.CYAN} offsets{' ' * (42 - len(str(found)) - len(str(total)))}│")
    log_info("╰────────────────────────────────────────────────────────────╯")

    return result


# ---------------------------------------------------------------------------
# VERSION READER
# ---------------------------------------------------------------------------

def read_process_version() -> Optional[str]:
    try:
        pe_header = read_exact(BASE, 0x1000)
        if not pe_header or pe_header[:2] != b"MZ":
            return None

        e_lfanew = struct.unpack_from("<I", pe_header, 0x3C)[0]
        if e_lfanew + 24 > len(pe_header) or pe_header[e_lfanew:e_lfanew + 4] != PE_SIGNATURE:
            return None

        coff_off = e_lfanew + 4
        num_sections = struct.unpack_from("<H", pe_header, coff_off + 2)[0]
        opt_size = struct.unpack_from("<H", pe_header, coff_off + 16)[0]
        opt_off = coff_off + 20
        size_of_image = struct.unpack_from("<I", pe_header, opt_off + 56)[0]

        section_table = opt_off + opt_size
        rsrc_va = 0
        rsrc_size = 0

        for i in range(num_sections):
            off = section_table + i * 40
            if off + 40 > len(pe_header):
                break
            name = pe_header[off:off + 8].split(b"\x00", 1)[0]
            if name == b".rsrc":
                rsrc_va = struct.unpack_from("<I", pe_header, off + 12)[0]
                rsrc_size = struct.unpack_from("<I", pe_header, off + 8)[0]
                if rsrc_size == 0:
                    rsrc_size = struct.unpack_from("<I", pe_header, off + 16)[0]
                break

        if rsrc_va and rsrc_size:
            scan_start = BASE + rsrc_va
            scan_end = scan_start + rsrc_size
        else:
            scan_start = BASE
            scan_end = BASE + (size_of_image if size_of_image > 0 else 0x4000000)

        sig = struct.pack("<I", 0xFEEF04BD)
        chunk_size = 0x100000

        for addr in range(scan_start, scan_end, chunk_size):
            to_read = min(chunk_size + 64, scan_end - addr)
            data = read_exact(addr, to_read)
            if not data:
                continue

            start = 0
            while True:
                idx = data.find(sig, start)
                if idx == -1 or idx + 52 > len(data):
                    break

                struc_ver = struct.unpack_from("<I", data, idx + 4)[0]
                if struc_ver != 0x00010000:
                    start = idx + 1
                    continue

                file_type = struct.unpack_from("<I", data, idx + 36)[0]
                if file_type not in (1, 2):
                    start = idx + 1
                    continue

                pv_ms = struct.unpack_from("<I", data, idx + 16)[0]
                pv_ls = struct.unpack_from("<I", data, idx + 20)[0]

                major = (pv_ms >> 16) & 0xFFFF
                minor = pv_ms & 0xFFFF
                build = (pv_ls >> 16) & 0xFFFF
                revision = pv_ls & 0xFFFF

                if 100 < major < 100000:
                    return f"{major}.{minor}.{build}.{revision}"

                start = idx + 1

        return None
    except Exception as e:
        log_warn(f"[!] Version read error: {e}")
        return None


# ---------------------------------------------------------------------------
# OUTPUT (C++ HEADER + JSON)
# ---------------------------------------------------------------------------

def write_header(
    offsets: Dict[str, int],
    out_dir: Path,
    version: Optional[str] = None,
    signature_order: Optional[List[str]] = None,
    decrypt_methods: Optional[Dict[str, str]] = None,
    decrypt_consts: Optional[Dict[str, int]] = None,
) -> Path:
    path = out_dir / "offsets.hpp"
    lines = [
        "#pragma once",
        "// Auto-generated by dumper.py",
        f"// Version: {version or 'unknown'}",
        f"// Base: 0x{BASE:X}",
        f"// Date: {time.strftime('%Y-%m-%d %H:%M:%S')}",
        "",
        "namespace offsets {",
    ]

    # ── Offset sabitleri ──────────────────────────────────────────────────────
    if signature_order:
        for name in signature_order:
            if name in offsets:
                lines.append(f"    constexpr uint64_t {name} = 0x{offsets[name]:X};")
    else:
        for name in sorted(offsets):
            lines.append(f"    constexpr uint64_t {name} = 0x{offsets[name]:X};")

    # ── Decrypt sabitleri (namespace offsets içinde) ──────────────────────────
    if decrypt_consts:
        lines += [
            "",
            "    // ── Decrypt constants (auto-extracted per patch) ────────────────────",
        ]
        for cname in sorted(decrypt_consts):
            lines.append(f"    constexpr uint64_t {cname} = 0x{decrypt_consts[cname]:X};")

    lines += ["}", ""]

    # ── Decrypt metodları (namespace dışında) ─────────────────────────────────
    if decrypt_methods:
        lines += [
            "// ---------------------------------------------------------------------------",
            "// Decrypt methods — otomatik olarak binary'den çıkarılmıştır",
            "// Her yamada sabitler değişebilir; metod her çalıştırmada yeniden üretilir.",
            "// _rotr için: #include <stdlib.h>  veya  #include <intrin.h>",
            "// ---------------------------------------------------------------------------",
            "",
            "struct Decrypt {",
        ]
        method_list = list(decrypt_methods.items())
        for i, (method_name, method_body) in enumerate(method_list):
            lines.append(method_body)
            if i < len(method_list) - 1:
                lines.append("")
        lines += ["};", ""]

    try:
        path.write_text("\n".join(lines), encoding="utf-8")
    except Exception as e:
        log_warn(f"[!] write_header yazma hatası: {e}")

    return path


def write_json(
    offsets: Dict[str, int],
    out_dir: Path,
    version: Optional[str] = None,
    decrypt_consts: Optional[Dict[str, int]] = None,
) -> Path:
    path = out_dir / "offsets.json"
    data = {
        "version": version or "unknown",
        "base": f"0x{BASE:X}",
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "offsets": {k: f"0x{v:X}" for k, v in sorted(offsets.items())},
    }
    if decrypt_consts:
        data["decrypt_constants"] = {
            k: f"0x{v:X}" for k, v in sorted(decrypt_consts.items())
        }
    path.write_text(json.dumps(data, indent=2), encoding="utf-8")
    return path


# ---------------------------------------------------------------------------
# MAIN
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(description="TslGame PE Dumper + Offset Scanner")
    parser.add_argument("-p", "--process", default="TslGame.exe")
    parser.add_argument("-d", "--device", default="fpga")
    parser.add_argument("-o", "--output", default="./out")
    parser.add_argument("--no-dump", action="store_true")
    parser.add_argument("--json", action="store_true", help="Also generate JSON output")
    args = parser.parse_args()

    out_dir = Path(args.output)
    out_dir.mkdir(parents=True, exist_ok=True)

    try:
        initialize(args.process, args.device)
    except Exception as exc:
        fail(f"Init error: {exc}")
        return

    version = read_process_version()
    if version:
        log_ok(f"[+] Process Version: {version}")
    else:
        log_warn("[!] Process Version: unknown")

    if not args.no_dump:
        log("\n[PE DUMP]")
        try:
            dump_pe(out_dir)
        except Exception as exc:
            log_warn(f"[!] PE dump hatası: {exc}")

    # ── Offset taraması ───────────────────────────────────────────────────────
    log("")
    try:
        offsets = scan_all_offsets()
    except Exception as exc:
        log_err(f"[-] Offset tarama hatası: {exc}")
        offsets = {}

    # ── Decrypt metod taraması ────────────────────────────────────────────────
    log("")
    try:
        decrypt_methods, decrypt_consts = scan_all_decrypt_methods()
    except Exception as exc:
        log_err(f"[-] Decrypt metod tarama hatası: {exc}")
        decrypt_methods, decrypt_consts = {}, {}

    # ── Çıktı dosyaları ───────────────────────────────────────────────────────
    log("")

    # Sıralama için signature isimlerinin listesini çıkar
    order = [item[0] for item in SIGNATURES]

    try:
        header_path = write_header(
            offsets, out_dir, version, order,
            decrypt_methods, decrypt_consts,
        )
        log_ok(f"[+] Header: {header_path}")
    except Exception as exc:
        log_err(f"[-] Header yazma hatası: {exc}")

    if args.json:
        try:
            json_path = write_json(offsets, out_dir, version, decrypt_consts)
            log_ok(f"[+] JSON: {json_path}")
        except Exception as exc:
            log_err(f"[-] JSON yazma hatası: {exc}")

    log_ok(f"[+] Resolved: {len(offsets)} offsets, {len(decrypt_methods)} decrypt metod")

    log_warn("\n[+] Done. Press Enter to exit...")
    input()


if __name__ == "__main__":
    main()