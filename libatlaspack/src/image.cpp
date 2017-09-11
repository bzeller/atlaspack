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

#include <AtlasPack/Image>

/**
 * \class AtlasPack::Image
 * \brief Represents informations about a texture to be painted into the Atlas
 */

namespace AtlasPack {

    class ImagePrivate {
        public:
            ImagePrivate (const std::string &p, const Size s)
                : path(p)
                , size(s){}

            std::string path;
            AtlasPack::Size size;
            bool valid = true;
    };

    /*!
     * \class Atlaspack::Image
     * Lightweight class to store image information like path and geometry for
     * atlas calculations. Makes it possible to calculate really big Atlas textures
     * without using a big amount of memory.
     */


    Image::Image()
        : p(new ImagePrivate("", AtlasPack::Size()))
    {
        p->valid = false;
    }

    Image::Image(const std::string &path, const AtlasPack::Size size)
        : p(new ImagePrivate(path, size))
    {

    }

    Image::Image(const Image &other)
        : p(new ImagePrivate(*other.p))
    {
    }

    Image::~Image()
    {
        if (p) delete p;
    }

    Image &Image::operator=(const Image &other)
    {
        *p = *other.p;
        return *this;
    }

    size_t Image::width() const
    {
        return p->size.width;
    }

    size_t Image::height() const
    {
        return p->size.height;
    }

    std::string Image::path() const
    {
        return p->path;
    }

    bool Image::isValid() const
    {
        return p->valid;
    }

}

