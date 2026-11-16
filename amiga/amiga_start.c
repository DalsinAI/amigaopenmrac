/* The Amiga port's start: the stack, the version string and PROGDIR:User,
 * where the game keeps its settings and its best laps' ghosts, made before
 * main(). Install-OpenMRac and GFOpenUp keep User when they update the game.
 * Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac. */
#include <sys/stat.h>
#include "amiga_start.h"

/* A Shell gives 4 KB of stack; SDL 2, Mesa's GL and the game want more.
 * libnix's startup sets this much up. */
unsigned long __stack = 256 * 1024;
static const char version[] __attribute__((used)) =
    "$VER: OpenMRac 1.3 (16.11.2026)";

int omr_make_drawer(const char *path)
{
	struct stat st;

	if (stat(path, &st) == 0)
		return S_ISDIR(st.st_mode) ? 0 : -1;
	return mkdir(path, 0755) == 0 ? 1 : -1;
}

#ifdef __amigaos__
static void __attribute__((constructor)) amiga_user_drawer(void)
{
	omr_make_drawer("PROGDIR:User");
}
#endif
