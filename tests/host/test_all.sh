#!/bin/sh
# Host tests (Linux, gcc/g++, SDL 2 and SDL2_image development files, GL;
# GLM, libpng and libjpeg development files too, if the host has them, for
# the comparisons in steps 4 and 5; Xvfb, libXtst and ImageMagick's import
# for step 7):
#  1. upstream and its data at the locked commits; amiga/patches apply;
#  2. tools/make_dat.py makes openmrac.dat: every picture, model and table
#     in it unchanged, every sound its WAVE file's samples;
#  3. the whole game builds with the host's compiler through
#     amiga/Makefile.amigaos, its <glm/...> answered by amiga/compat/glm
#     only;
#  4. test_glm prints every GLM call the game makes on our stand-in and, on
#     a host with GLM, on GLM itself: the outputs must be the same;
#  5. test_pict reads every picture of the data on our SDL_image reader
#     and, on a host with libpng and libjpeg, on upstream's readers: the
#     pixels must be the same;
#  6. test_start: the User drawer;
#  7. the game is played on a virtual X display through the keyboard, in a
#     drawer laid out as on the Amiga (PROGDIR:Data and PROGDIR:User are
#     plain directory names here): the settings window shows the Amiga
#     defaults, OK, the menu, Race, GO!, then the accelerator held; the
#     settings must be written to PROGDIR:User and the car must move.
# Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
. "$ROOT/package/openmrac.lock"
T=${WORK:-"${TMPDIR:-/tmp}/amigaopenmrac-test"}
mkdir -p "$T"
cd "$ROOT"
ok() { echo "ok: $*"; }
fetch() {   # fetch DIR URL COMMIT
    if [ "$(git -C "$1" rev-parse HEAD 2>/dev/null)" != "$3" ]; then
        rm -rf "$1"; mkdir -p "$1"
        git -C "$1" init -q
        git -C "$1" fetch -q --depth 1 "$2" "$3"
        git -C "$1" checkout -q FETCH_HEAD
    fi
    git -C "$1" checkout -q -- . && git -C "$1" clean -qfdx
}

# 1. upstream + patches
OMR="$T/OpenMRac"; DATA="$T/OpenMRac-data"
fetch "$OMR" "$OMR_URL" "$OMR_COMMIT"
fetch "$DATA" "$OMR_DATA_URL" "$OMR_DATA_COMMIT"
for p in amiga/patches/*.patch; do git -C "$OMR" apply "$ROOT/$p"; done
ok "patches apply to $OMR_COMMIT"

# 2. openmrac.dat
DAT="$T/openmrac.dat"
python3 tools/make_dat.py "$DATA" "$DAT" > /dev/null
X="$T/dat"; rm -rf "$X"; mkdir -p "$X"; tar -xf "$DAT" -C "$X"
n=0
for f in "$DATA"/*.jpg "$DATA"/*.png "$DATA"/*.3dm "$DATA"/*.3mt "$DATA"/*.cmo "$DATA"/*.def; do
    cmp -s "$f" "$X/$(basename "$f")" || { echo "FAIL: $(basename "$f") differs in openmrac.dat"; exit 1; }
    n=$((n + 1))
done
for f in "$DATA"/*.wav; do
    r="$X/$(basename "$f" .wav).raw"
    # the samples as Python's own wave module reads them
    python3 -c 'import sys, wave; w = wave.open(sys.argv[1]); assert w.getsampwidth() == 2; sys.stdout.buffer.write(w.readframes(w.getnframes()))' "$f" \
        | cmp -s - "$r" || { echo "FAIL: $(basename "$r") is not $(basename "$f")'s samples"; exit 1; }
    n=$((n + 1))
done
[ "$(tar -tf "$DAT" | wc -l)" = "$n" ] || { echo "FAIL: openmrac.dat has other files"; exit 1; }
ok "openmrac.dat: $n files, each its source's bytes or samples"

# 3. the build
B="$T/build"
make -s -f amiga/Makefile.amigaos -j8 OMR="$OMR" BUILD="$B" CC=gcc CXX=g++ CPU= NOIX= SDL2KIT=/usr OPT=-O1 \
    LIBS="$(sdl2-config --libs) -lSDL2_image -lGL -lm" "$B/OpenMRac.debug" > "$T/make.log" 2>&1 \
    || { tail -30 "$T/make.log"; exit 1; }
for f in main shadermng gamemng 3dm; do
    if g++ -std=c++20 -M -D__amigaos__ -DUSE_MINIAL -Iamiga/compat -I"$OMR/src" -Iamiga $(sdl2-config --cflags) \
        "$OMR/src/$f.cpp" | tr ' ' '\n' | grep -v amiga/compat | grep -q "/glm/"; then
        echo "FAIL: $f.cpp reaches a GLM header that is not ours"; exit 1
    fi
done
ok "game builds on the host, on our GLM stand-in only"

# 4. GLM
g++ -std=c++20 -O2 -Wall -Iamiga/compat -o "$T/test_glm" tests/host/test_glm.cpp
"$T/test_glm" > "$T/glm.ours"
if [ -f /usr/include/glm/glm.hpp ]; then
    g++ -std=c++20 -O2 -Wall -o "$T/test_glm_real" tests/host/test_glm.cpp
    "$T/test_glm_real" > "$T/glm.real"
    diff "$T/glm.real" "$T/glm.ours" || { echo "FAIL: our GLM stand-in and GLM differ"; exit 1; }
    ok "test_glm: $(wc -l < "$T/glm.ours") results, the same on GLM to the last bit"
else
    echo "skip: no GLM on this host for the comparison"
fi

# 5. the pictures
CF="$(sdl2-config --cflags)"; LF="$(sdl2-config --libs)"
g++ -std=c++20 -O1 -I"$OMR/src" $CF -o "$T/test_pict" tests/host/test_pict.cpp amiga/compat/pict2_sdlimage.cpp \
    "$OMR/src/pict2.cpp" $LF -lSDL2_image
"$T/test_pict" "$DATA"/*.png "$DATA"/*.jpg > "$T/pict.ours"
if [ -f /usr/include/png.h ] && [ -f /usr/include/jpeglib.h ]; then
    g++ -std=c++20 -O1 -I"$OMR/src" $CF -o "$T/test_pict_up" tests/host/test_pict.cpp "$OMR/src/pict2_png.cpp" \
        "$OMR/src/pict2_jpeg.cpp" "$OMR/src/pict2.cpp" $LF -lpng -ljpeg
    "$T/test_pict_up" "$DATA"/*.png "$DATA"/*.jpg > "$T/pict.up"
    diff "$T/pict.up" "$T/pict.ours" || { echo "FAIL: our picture reader and upstream's differ"; exit 1; }
    ok "test_pict: $(wc -l < "$T/pict.ours") pictures, the same pixels as libpng and libjpeg give"
else
    echo "skip: no libpng or libjpeg on this host for the comparison"
fi

# 6. the User drawer
gcc -O1 -Wall -Iamiga -o "$T/test_start" tests/host/test_start.c amiga/amiga_start.c
"$T/test_start" "$T"

# 7. played through the keyboard on a virtual display
if command -v Xvfb >/dev/null && command -v import >/dev/null && python3 -c 'import ctypes; ctypes.CDLL("libXtst.so.6")' 2>/dev/null; then
    R="$T/run"
    rm -rf "$R"; mkdir -p "$R/PROGDIR:Data" "$R/PROGDIR:User"
    cp "$DAT" "$R/PROGDIR:Data/openmrac.dat"
    D=:$((70 + $$ % 20))
    Xvfb "$D" -screen 0 800x600x24 >/dev/null 2>&1 &
    XP=$!
    sleep 2
    (cd "$R" && DISPLAY=$D SDL_AUDIODRIVER=dummy timeout -s KILL 60 "$B/OpenMRac.debug") > "$T/play.out" 2>&1 &
    GP=$!
    sleep 5
    python3 "$HERE/xkeys.py" "$D" shot:"$T/settings.png" Return sleep:6000 shot:"$T/menu.png" Return sleep:1500 \
        Down Down Down Down Down Down sleep:500 Return sleep:12000 shot:"$T/race0.png" Up:4000 shot:"$T/race1.png"
    kill -9 "$GP" 2>/dev/null || true; kill "$XP" 2>/dev/null || true
    wait 2>/dev/null || true
    S="$R/PROGDIR:User/settings-sdl2.dat"
    [ -f "$S" ] || { echo "FAIL: no settings in PROGDIR:User"; cat "$T/play.out"; exit 1; }
    grep -q "^screen_x 640" "$S" && grep -q "^freq 0" "$S" || { echo "FAIL: not the Amiga defaults"; cat "$S"; exit 1; }
    if cmp -s "$T/race0.png" "$T/race1.png"; then echo "FAIL: nothing moved in the race"; exit 1; fi
    ok "played on $D: settings (Amiga defaults, in PROGDIR:User), menu, race, the car drives"
else
    echo "skip: no Xvfb, libXtst or import for the played test"
fi
echo "all host tests passed"
