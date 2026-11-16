/* Pict2's JPEG and PNG readers on SDL_image, for the Amiga port: the
 * pictures are decoded by GFOpenGPU's SDL2_image.module (which hands JPEG
 * and PNG to the GPU's cores where it can) instead of a libjpeg and a
 * libpng built into the game. It replaces upstream's pict2_jpeg.cpp and
 * pict2_png.cpp and gives what they gave: 8-bit RGBA, bottom row first (as
 * OpenGL wants it); a JPEG's alpha is 255, a PNG's is its own (a paletted
 * PNG's transparent colour too).
 * Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac. */
#include <SDL.h>
#include <SDL_image.h>
#include "pict2.h"

static SDL_Surface* omr_decode(bool bfile, const void* fname_data, unsigned int size)
{
    SDL_RWops* rw = bfile ? SDL_RWFromFile((const char*)fname_data, "rb")
                          : SDL_RWFromConstMem(fname_data, (int)size);
    if (!rw)
        return nullptr;
    SDL_Surface* s = IMG_Load_RW(rw, 1);
    if (!s)
        return nullptr;
    /* RGBA32: the bytes R, G, B, A in memory, on either byte order */
    SDL_Surface* c = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(s);
    return c;
}

static int omr_load(Pict2& p, bool bfile, const void* fname_data, unsigned int size, bool opaque)
{
    SDL_Surface* c = omr_decode(bfile, fname_data, size);
    if (!c)
        return 0;
    std::vector<unsigned char> px((size_t)c->w * c->h * 4);
    SDL_LockSurface(c);
    for (int y = 0; y < c->h; ++y) {
        const unsigned char* row = (const unsigned char*)c->pixels + (size_t)y * c->pitch;
        unsigned char* to = px.data() + (size_t)(c->h - 1 - y) * c->w * 4;
        memcpy(to, row, (size_t)c->w * 4);
        if (opaque)
            for (int x = 0; x < c->w; ++x)
                to[x * 4 + 3] = 0xff;
    }
    SDL_UnlockSurface(c);
    p.assign(c->w, c->h, px.data());
    SDL_FreeSurface(c);
    return 1;
}

int Pict2::loadpng(const void* data, unsigned int size) { return omr_load(*this, false, data, size, false); }
int Pict2::loadpng(const char* fname) { return omr_load(*this, true, fname, 0, false); }
int Pict2::loadjpeg(const void* data, unsigned int size) { return omr_load(*this, false, data, size, true); }
int Pict2::loadjpeg(const char* fname) { return omr_load(*this, true, fname, 0, true); }
