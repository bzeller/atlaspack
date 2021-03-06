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
#ifndef ATLASPACK_BACKEND_INCLUDED
#define ATLASPACK_BACKEND_INCLUDED

#include <AtlasPack/atlaspack_global.h>
#include <AtlasPack/PaintDevice>
#include <AtlasPack/Image>
#include <AtlasPack/Dimension>
#include <string>
#include <memory>
#include <functional>

namespace AtlasPack {

class ATLASPACK_EXPORT Backend {
    public:
        virtual ~Backend();
        virtual bool supportsImageType (const std::string &extension) const = 0;
        virtual std::shared_ptr<PaintDevice> createPaintDevice (const Size &reserveSize) const = 0;
        virtual Image readImageInformation (const std::string &path) const = 0;
};

}

#endif
