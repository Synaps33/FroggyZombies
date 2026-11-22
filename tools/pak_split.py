#!/usr/bin/env python3
"""
pak_split.py - Split a large Quake .pak file into multiple files smaller than 32 MB
to comply with SF2000 / GB300 FAT32 filesystem seek limits.
"""

import os
import struct
import sys

def split_pak(input_path, output_dir, max_size_mb=28):
    max_bytes = max_size_mb * 1024 * 1024
    with open(input_path, 'rb') as f:
        data = f.read()

    if data[:4] != b'PACK':
        print(f"Error: {input_path} is not a valid Quake PACK file.")
        sys.exit(1)

    dirofs, dirlen = struct.unpack('<ii', data[4:12])
    num_files = dirlen // 64
    print(f"Loaded {input_path}: {len(data) / (1024*1024):.2f} MB, {num_files} files.")

    files = []
    for i in range(num_files):
        entry = data[dirofs + i*64 : dirofs + (i+1)*64]
        name = entry[:56].split(b'\0')[0].decode('latin1')
        pos, length = struct.unpack('<ii', entry[56:64])
        files.append((name, pos, length))

    os.makedirs(output_dir, exist_ok=True)
    # Split files into chunks
    chunks = []
    current_chunk = []
    current_size = 12

    for name, pos, length in files:
        if current_chunk and (current_size + length + 64 > max_bytes):
            chunks.append(current_chunk)
            current_chunk = []
            current_size = 12
        current_chunk.append((name, data[pos:pos+length]))
        current_size += length + 64

    if current_chunk:
        chunks.append(current_chunk)

    for idx, chunk in enumerate(chunks):
        out_name = "nzp.pak" if idx == 0 else f"nzp{idx+1}.pak"
        out_path = os.path.join(output_dir, out_name)
        file_data = b""
        directory = b""
        header_size = 12
        data_offset = header_size

        for name, content in chunk:
            directory += struct.pack('<56sii', name.encode('latin1'), data_offset + len(file_data), len(content))
            file_data += content

        dirofs = header_size + len(file_data)
        dirlen = len(directory)
        header = struct.pack('<4sii', b'PACK', dirofs, dirlen)

        with open(out_path, 'wb') as out_f:
            out_f.write(header)
            out_f.write(file_data)
            out_f.write(directory)

        print(f"Wrote {out_name}: {os.path.getsize(out_path) / (1024*1024):.2f} MB ({len(chunk)} files)")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: pak_split.py <input.pak> <output_dir> [max_mb]")
        sys.exit(1)
    max_mb = int(sys.argv[3]) if len(sys.argv) > 3 else 28
    split_pak(sys.argv[1], sys.argv[2], max_mb)
