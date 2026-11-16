#!/bin/sh
# The release archive and the GFOpenUp part, for Cradle's stove_build
# (target package):
#  1. upstream OpenMRac and OpenMRac-data, fetched at the commits
#     package/openmrac.lock names (neither is kept in this repository),
#     with amiga/patches applied to the code;
#  2. openmrac.dat made from the data by tools/make_dat.py;
#  3. the game built by amiga/Makefile.amigaos;
#  4. the drawer, its icon and the Installer script beside it packed as
#     NAME-VERSION-68040.lha (tools/pack_dist.py), with its .sha256 and a
#     short .readme;
#  5. OUT/Parts/OpenMRac, a GFOpenUp part: its Part file and the drawer's
#     files for SYS:Games/OpenMRac, installed with
#       GFOpenUpTool INSTALL Parts/OpenMRac
#     It needs the GFOpenGPU part, whose SDL 2 and GL modules draw it.
# Nothing is published from here.
# Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac.
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/.." && pwd)
. "$HERE/package.env"
. "$HERE/openmrac.lock"
OUT=${OUT:-"$ROOT/build-amiga-package"}
mkdir -p "$OUT"
OUT=$(CDPATH= cd -- "$OUT" && pwd)
STOVE=${STOVE:-"$HOME/AmiCradle/kitchen/stoves/os32-gcc16-open6"}
# outside the checkout: Cradle cleans it (git clean -ffdx) before every job
WORK=${WORK:-"$HOME/.cache/amigaopenmrac"}
mkdir -p "$WORK"
cd "$ROOT"

fetch() {   # fetch DIR URL COMMIT
    if [ "$(git -C "$1" rev-parse HEAD 2>/dev/null)" != "$3" ]; then
        rm -rf "$1"; mkdir -p "$1"
        git -C "$1" init -q
        git -C "$1" fetch -q --depth 1 "$2" "$3"
        git -C "$1" checkout -q FETCH_HEAD
    fi
    git -C "$1" checkout -q -- . && git -C "$1" clean -qfdx
}

# 1. upstream at the locked commits, patched afresh every time
OMR="$WORK/OpenMRac"; DATA="$WORK/OpenMRac-data"
fetch "$OMR" "$OMR_URL" "$OMR_COMMIT"
fetch "$DATA" "$OMR_DATA_URL" "$OMR_DATA_COMMIT"
for p in "$ROOT"/amiga/patches/*.patch; do
    git -C "$OMR" apply "$p"
done

# 2. the data file
python3 "$ROOT/tools/make_dat.py" "$DATA" "$WORK/openmrac.dat"

# 3. the game (a stove's compiler is in its prefix/, or STOVE is the prefix)
if [ -x "$STOVE/prefix/bin/m68k-amigaos-gcc" ]; then PREFIX="$STOVE/prefix"; else PREFIX="$STOVE"; fi
rm -rf "$DIST"
make -f amiga/Makefile.amigaos OMR="$OMR" DAT="$WORK/openmrac.dat" STOVE="$STOVE" PREFIX="$PREFIX" -j4 $TARGET \
    > "$OUT/build-output.txt" 2>&1 || { tail -n 60 "$OUT/build-output.txt"; exit 1; }

# 4. the archive
python3 "$ROOT/tools/pack_dist.py" --dist "$DIST" --name "$NAME" --out "$OUT"

# 5. the GFOpenUp part
sh "$ROOT/package/write-part.sh" "$DIST/$NAME" "$OUT/Parts/$NAME" "$NAME" "$PORT_VERSION" \
    "OpenMRac: split-screen 3D car racing for 1 to 4 players"
echo "part: $(grep -c '^copy' "$OUT/Parts/$NAME/Part") files, $(du -sk "$OUT/Parts/$NAME" | cut -f1) KB"
ls -l "$DIST/$NAME/$NAME"
