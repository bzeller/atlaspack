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
#include <iostream>
#include <boost/algorithm/string.hpp>

MagickBackend::MagickBackend()
{
    //has to be called before ANY magick can be used
    Magick::InitializeMagick(NULL);
}

bool MagickBackend::supportsImageType(const std::string &extension) const
{
    std::string ext = boost::algorithm::to_lower_copy(extension);
    return (ext == ".jpg" || ext == ".png" || ext == ".svg");
}

std::shared_ptr<AtlasPack::PaintDevice> MagickBackend::createPaintDevice(const AtlasPack::Size &reserveSize) const
{
    return std::make_shared<MagickPaintDevice>(reserveSize);
}

AtlasPack::Image MagickBackend::readImageInformation(const std::string &path) const
{
    Magick::Image img;
    try {
        //read just enough of the file to figure out the geometry, right now thats all we need to know
        img.ping( path );

        AtlasPack::Size geom(img.columns(), img.rows());
        return AtlasPack::Image (path, geom);

    }
    catch( Magick::Exception &error )
    {
        std::cerr << "Unable to read file: " << path << " " <<error.what() << std::endl;
    }
    return AtlasPack::Image();
}



MagickPaintDevice::MagickPaintDevice(const AtlasPack::Size &reserveSize)
    : m_painter(new Magick::Image(Magick::Geometry(reserveSize.width, reserveSize.height),"White"))
{

}

MagickPaintDevice::~MagickPaintDevice()
{
    if (m_painter) delete m_painter;
}

bool MagickPaintDevice::paintImageFromFile(AtlasPack::Pos topleft, std::string filename)
{
    try {
        Magick::Image input;
        input.read(filename);

        m_painter->composite(input, topleft.x, topleft.y);
        return true;

    } catch( Magick::Exception &error_ ) {
        std::cerr << "Caught exception: " << error_.what() << std::endl;
        std::cerr << "Unable to compose file: " << filename << std::endl;
    }
    return false;
}

bool MagickPaintDevice::exportToFile(std::string filename)
{
    try {
        m_painter->write(filename);
        return true;
    } catch( Magick::Exception &error_ ) {
        std::cerr << "Caught exception: " << error_.what() << std::endl;
        std::cerr << "Unable to draw file: " << filename << std::endl;
    }
    return false;
}
