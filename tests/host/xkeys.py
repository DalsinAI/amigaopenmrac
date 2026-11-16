# Host test helper (tests/host/test_all.sh, step 5). Copyright (c) 2026
# AmiCradle Team. BSD-2-Clause, like OpenMRac.
# Presses keys on an X display through XTest (ctypes; no headers needed).
#   keys.py DISPLAY step...   step: KEYSYM[:holdms] or sleep:ms or shot:file
import ctypes, sys, time, subprocess
x = ctypes.CDLL("libX11.so.6"); t = ctypes.CDLL("libXtst.so.6")
x.XOpenDisplay.restype = ctypes.c_void_p
x.XStringToKeysym.restype = ctypes.c_ulong
x.XKeysymToKeycode.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
t.XTestFakeKeyEvent.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_int, ctypes.c_ulong]
x.XFlush.argtypes = [ctypes.c_void_p]
d = x.XOpenDisplay(sys.argv[1].encode())
for s in sys.argv[2:]:
    k, _, v = s.partition(':')
    if k == 'sleep': time.sleep(int(v) / 1000); continue
    if k == 'shot':
        subprocess.run(['import', '-display', sys.argv[1], '-window', 'root', v]); continue
    kc = x.XKeysymToKeycode(d, x.XStringToKeysym(k.encode()))
    t.XTestFakeKeyEvent(d, kc, 1, 0); x.XFlush(d)
    time.sleep(int(v or 120) / 1000)
    t.XTestFakeKeyEvent(d, kc, 0, 0); x.XFlush(d)
    time.sleep(0.25)
