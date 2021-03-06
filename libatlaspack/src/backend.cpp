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

#include <AtlasPack/Backend>

namespace AtlasPack {

/**
 * \class AtlasPack::Backend
 * The \a Backend class is the link between the packing algorithms and the means
 * of how libatlaspack paints the resulting texture atlas. It can be used to support
 * custom image processing libraries.
 *
 * \note Painting the images has to be thread safe, since \sa AtlasPack::TextureAtlasPacker uses
 * multiple threads to paint the images
 */

/**
  * \fn AtlasPack::Backend::supportsImageType
  * Checks if the Backend implementation supports the image type given by \a extension.
  * The \a extension string has to be in boost::filesystem format e.g. ".png"
  */

/**
  * \fn AtlasPack::Backend::createPaintDevice
  * Creates a new instance of \sa AtlasPack::PaintDevice and initializes the painter geometry with
  * \a size.
  */

/**
  * \fn AtlasPack::Backend::readImageInformation
  * Opens the image specified by \a path and reads the minimal required information
  * to fill the return value type \sa AtlasPack::Image.
  *
  * \note depending on the Backend implementation this might need to read the full image, however if
  *       possible the Backend should only read as little from the file as needed
  */

Backend::~Backend()
{

}

}
