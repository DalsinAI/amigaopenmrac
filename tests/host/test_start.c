/* Host test of amiga/amiga_start.c's omr_make_drawer().
 *   test_start <scratch dir>
 * Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac. */
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "amiga_start.h"

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); fails++; } } while (0)

int main(int argc, char **argv)
{
	const char *dir = argc > 1 ? argv[1] : ".";
	char d[512];
	struct stat st;
	FILE *f;

	snprintf(d, sizeof d, "%s/User", dir);
	rmdir(d);
	CHECK(omr_make_drawer(d) == 1);
	CHECK(stat(d, &st) == 0 && S_ISDIR(st.st_mode));
	CHECK(omr_make_drawer(d) == 0);
	snprintf(d, sizeof d, "%s/afile", dir);
	f = fopen(d, "wb");
	if (f)
		fclose(f);
	CHECK(omr_make_drawer(d) == -1);
	remove(d);
	printf("%s: %d failed\n", argv[0], fails);
	return fails != 0;
}
