#!/usr/bin/env python3
"""openmrac.dat from the OpenMRac-data checkout, as upstream's Makefile makes
it, without sox: each .wav (16-bit PCM) becomes a .raw of its samples as
they are in the file (little-endian; the game swaps them on a big-endian
68k, src/soundmng.h), then GNU tar appends the files one by one in `ls`
order with upstream's owner, group, time and mode.
    make_dat.py DATA_DIR OUT_DAT
Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac."""
import os, shutil, struct, subprocess, sys, tempfile

def wav_samples(path):
    with open(path, 'rb') as f:
        d = f.read()
    if d[:4] != b'RIFF' or d[8:12] != b'WAVE':
        raise SystemExit(f'{path}: not a WAVE file')
    i, fmt = 12, None
    while i + 8 <= len(d):
        cid, size = d[i:i + 4], struct.unpack('<I', d[i + 4:i + 8])[0]
        body = d[i + 8:i + 8 + size]
        if cid == b'fmt ':
            fmt = struct.unpack('<HHIIHH', body[:16])
        elif cid == b'data':
            if not fmt or fmt[0] != 1 or fmt[5] != 16:
                raise SystemExit(f'{path}: not 16-bit PCM')
            return body
        i += 8 + size + (size & 1)
    raise SystemExit(f'{path}: no data')

def main():
    src, out = sys.argv[1], os.path.abspath(sys.argv[2])
    with tempfile.TemporaryDirectory() as t:
        for n in os.listdir(src):
            if n.endswith(('.jpg', '.png', '.3dm', '.3mt', '.cmo', '.def')):
                shutil.copy2(os.path.join(src, n), t)
            elif n.endswith('.wav'):
                with open(os.path.join(t, n[:-4] + '.raw'), 'wb') as f:
                    f.write(wav_samples(os.path.join(src, n)))
        # upstream: ls *.jpg *.png *.3dm *.3mt *.cmo *.def *.raw (each glob sorted in the C locale)
        names = []
        for ext in ('jpg', 'png', '3dm', '3mt', 'cmo', 'def', 'raw'):
            names += sorted((n for n in os.listdir(t) if n.endswith('.' + ext)), key=lambda s: s.encode())
        if os.path.exists(out):
            os.remove(out)
        env = dict(os.environ, LC_ALL='C')
        for n in names:
            subprocess.run(['tar', '--owner=root:0', '--group=root:0', '--mtime=UTC 1970-01-01 00:00:00',
                            '--mode=go=rX,u+rw,a-s', '-rf', out, n], cwd=t, check=True, env=env)
    print(f'{out}: {len(names)} files')

main()
