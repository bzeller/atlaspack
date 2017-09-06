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

#include "magickbackend.h"

#include <AtlasPack/Dimension>
#include <Magick++.h>
#include <iostream>

MagickBackend::MagickBackend()
{
    //has to be called before ANY magick can be used
    Magick::InitializeMagick(NULL);
}

std::shared_ptr<AtlasPack::PaintDevice> MagickBackend::createPaintDevice(const std::string &filename, const AtlasPack::Size &reserveSize) const
{
    return std::shared_ptr<AtlasPack::PaintDevice>();
}

AtlasPack::Image MagickBackend::readImageInformation(const std::string &path) const
{
    Magick::Image img;
    try {
        //read just enough of the file to figure out the geometry, right now thats all we need to know
        img.ping( path );

        AtlasPack::Size geom;
        geom.width = img.columns();
        geom.height = img.rows();

        return AtlasPack::Image (path, geom);

    }
    catch( Magick::Exception &error_ )
    {
        std::cout << "Caught exception: " << error_.what() << std::endl;
        std::cout << "Unable to read file: " << path << std::endl;
    }
    return AtlasPack::Image();
}



MagickPaintDevice::MagickPaintDevice(const std::string &filename, const AtlasPack::Size &reserveSize)
{

}

bool MagickPaintDevice::paintImageFromFile(AtlasPack::Pos topleft, std::string filename)
{
    return false;
}
