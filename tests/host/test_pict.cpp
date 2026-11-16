// Host test of amiga/compat/pict2_sdlimage.cpp: every picture in
// OpenMRac-data read from memory, as the game reads them from
// openmrac.dat. Built twice by tests/host/test_all.sh, once on our reader
// (SDL_image) and once on upstream's pict2_png.cpp and pict2_jpeg.cpp
// (libpng, libjpeg); both print each picture's size and a hash of its RGBA
// pixels, and the outputs must be the same. With "dump DIR" it also writes
// each picture's pixels to DIR, for a closer look when they differ.
//   test_pict FILE... [dump DIR]
// Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac.
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <SDL.h>
#include "pict2.h"

static std::vector<unsigned char> slurp(const char* f)
{
    std::vector<unsigned char> d;
    FILE* in = fopen(f, "rb");
    if (!in)
        return d;
    unsigned char buf[65536];
    size_t n;
    while ((n = fread(buf, 1, sizeof buf, in)) > 0)
        d.insert(d.end(), buf, buf + n);
    fclose(in);
    return d;
}

int main(int argc, char** argv)
{
    const char* dump = nullptr;
    int fails = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "dump") && i + 1 < argc) { dump = argv[++i]; continue; }
        std::string name(argv[i]);
        std::vector<unsigned char> d = slurp(argv[i]);
        Pict2 p;
        bool jpeg = name.size() > 4 && name.compare(name.size() - 4, 4, ".jpg") == 0;
        int r = jpeg ? p.loadjpeg(d.data(), (unsigned)d.size()) : p.loadpng(d.data(), (unsigned)d.size());
        unsigned long long h = 1469598103934665603ULL;
        for (int k = 0; k < p.w() * p.h() * 4; ++k)
            h = (h ^ p.c_px()[k]) * 1099511628211ULL;
        // upstream: 0 failed, 1 read, more than 1 read with a warning (its JPEG
        // reader says 5 for every picture here)
        printf("%s %s %dx%d %016llx\n", name.substr(name.find_last_of('/') + 1).c_str(), r >= 1 ? "read" : "FAILED", p.w(), p.h(), h);
        if (r < 1)
            fails++;
        if (dump && r >= 1) {
            std::string o = std::string(dump) + "/" + name.substr(name.find_last_of('/') + 1) + ".rgba";
            FILE* out = fopen(o.c_str(), "wb");
            if (out) {
                fwrite(p.c_px(), 1, (size_t)p.w() * p.h() * 4, out);
                fclose(out);
            }
        }
    }
    return fails != 0;
}
