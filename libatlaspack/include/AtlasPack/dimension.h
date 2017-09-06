/*
 * MIT License
 *
 * Copyright (c) 2017 Benjamin Zeller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef ATLASPACK_DIMENSION_INCLUDED
#define ATLASPACK_DIMENSION_INCLUDED

#include <stddef.h>

namespace AtlasPack {

struct Size {
    Size (size_t w = 0, size_t h = 0)
        : width(w), height(h) {}
    size_t width  = 0;
    size_t height = 0;
};

struct Pos {
    Pos (size_t _x = 0, size_t _y = 0)
        :x(_x), y(_y) {}
    size_t x = 0;
    size_t y = 0;
};

struct Rect {
    Rect (Pos pos, Size s)
        : topLeft(pos), size(s) {}
    Pos  topLeft;
    Size size;
};

}

#endif
