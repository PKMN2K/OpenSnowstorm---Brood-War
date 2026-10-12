#!/usr/bin/env python3
"""Replace a placed Marine in an extracted scenario.chk with custom unit ID 228.

An existing Brood War UMS map is required. Extract staredit/scenario.chk from
its SCX/SCM archive and repack the generated CHK into a copied archive.
Does not distribute original game assets.
"""
import argparse
import pathlib
import struct

def patch_chk(data: bytes, owner: int = 0) -> bytes:
    pos = 0
    result = bytearray(data)
    changed = False
    while pos + 8 <= len(data):
        tag = data[pos:pos + 4]
        size = struct.unpack_from("<I", data, pos + 4)[0]
        begin, end = pos + 8, pos + 8 + size
        if end > len(data):
            raise ValueError("truncated CHK section")
        if tag == b"UNIT":
            if size % 36:
                raise ValueError("invalid UNIT section length")
            for offset in range(begin, end, 36):
                unit_type = struct.unpack_from("<H", data, offset + 8)[0]
                unit_owner = data[offset + 16]
                if unit_type == 0 and unit_owner == owner and not changed:
                    struct.pack_into("<H", result, offset + 8, 228)
                    changed = True
        pos = end
    if pos != len(data):
        raise ValueError("trailing invalid CHK bytes")
    if not changed:
        raise ValueError("no placed Marine (unit ID 0) for the selected owner")
    return bytes(result)

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("input_chk", type=pathlib.Path)
    p.add_argument("output_chk", type=pathlib.Path)
    p.add_argument("--owner", type=int, default=0, choices=range(12), metavar="0..11")
    args = p.parse_args()
    if args.input_chk.resolve() == args.output_chk.resolve():
        p.error("output must differ from input")
    args.output_chk.write_bytes(patch_chk(args.input_chk.read_bytes(), args.owner))
    print("Created custom Marine UNIT record (type 228), owner", args.owner)

if __name__ == "__main__":
    main()
