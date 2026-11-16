/* The Amiga port's start (amiga_start.c).
 * Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac. */
#ifndef OMR_AMIGA_START_H
#define OMR_AMIGA_START_H
#ifdef __cplusplus
extern "C" {
#endif
/* Makes a drawer if it is not there: 1 made, 0 already there, -1 failed
 * (or a file of that name is in the way). */
int omr_make_drawer(const char *path);
#ifdef __cplusplus
}
#endif
#endif
