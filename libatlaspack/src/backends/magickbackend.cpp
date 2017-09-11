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

#include <AtlasPack/Backends/MagickBackend>
#include <AtlasPack/Dimension>


#include <Magick++.h>

#include <iostream>
#include <boost/algorithm/string.hpp>

namespace AtlasPack {
namespace Backends {

/*!
 * \class AtlasPack::Backends::MagickBackend
 *
 * Implements the \sa AtlasPack::Backend interface to support painting with
 * the Magick++ library.
 *
 * \todo Move this into a plugin that can be loaded on runtime, allows it more easily
 *       to switch between different backend implementations
 */

MagickBackend::MagickBackend()
{
    //has to be called before ANY magick can be used
    Magick::InitializeMagick(NULL);
}

/*!
 * \brief MagickBackend::supportsImageType
 * Currenty does check statically if images are supported without
 * asking Magick++.
 *
 * \sa AtlasBackend::Backend::supportsImageType
 * \todo Use Magick++ API to check if the library actually has drivers for the image types
 */
bool MagickBackend::supportsImageType(const std::string &extension) const
{
    std::string ext = boost::algorithm::to_lower_copy(extension);
    return (ext == ".jpg" || ext == ".png" || ext == ".svg");
}

/*!
 * \brief MagickBackend::createPaintDevice
 * Reimplements the createPaintDevice function from \sa AtlasBackend::Backend
 * \sa AtlasBackend::Backend::createPaintDevice
 */
std::shared_ptr<AtlasPack::PaintDevice> MagickBackend::createPaintDevice(const AtlasPack::Size &reserveSize) const
{
    return std::make_shared<MagickPaintDevice>(reserveSize);
}

/*!
 * \brief MagickBackend::readImageInformation
 * Reimplements the readImageInformation function from \sa AtlasBackend::Backend
 * \sa AtlasBackend::Backend::readImageInformation
 */
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


class MagickPaintDevicePrivate {
    public:
        MagickPaintDevicePrivate(const Magick::Geometry &size)
            :m_painter(new Magick::Image(size, "White")) { }
        std::shared_ptr<Magick::Image> m_painter;
};

/*!
 * \class AtlasPack::Backends::MagickPaintDevice
 *
 * Implements the \sa AtlasPack::PaintDevice interface to support painting with
 * the Magick++ library.
 *
 */
MagickPaintDevice::MagickPaintDevice(const AtlasPack::Size &reserveSize)
    : p(new MagickPaintDevicePrivate(Magick::Geometry(reserveSize.width, reserveSize.height)))
{

}

MagickPaintDevice::~MagickPaintDevice()
{
    if (p) delete p;
}

/*!
 * \brief MagickPaintDevice::paintImageFromFile
 * Reimplements the paintImageFromFile function from \sa AtlasBackend::MagickPaintDevice
 * \sa AtlasBackend::MagickPaintDevice::paintImageFromFile
 */
bool MagickPaintDevice::paintImageFromFile(AtlasPack::Pos topleft, std::string filename)
{
    try {
        Magick::Image input;
        input.read(filename);

        p->m_painter->composite(input, topleft.x, topleft.y);
        return true;

    } catch( Magick::Exception &error_ ) {
        std::cerr << "Caught exception: " << error_.what() << std::endl;
        std::cerr << "Unable to compose file: " << filename << std::endl;
    }
    return false;
}

/*!
 * \brief MagickPaintDevice::exportToFile
 * Reimplements the exportToFile function from \sa AtlasBackend::MagickPaintDevice
 * \sa AtlasBackend::MagickPaintDevice::exportToFile
 */
bool MagickPaintDevice::exportToFile(std::string filename)
{
    try {
        p->m_painter->write(filename);
        return true;
    } catch( Magick::Exception &error_ ) {
        std::cerr << "Caught exception: " << error_.what() << std::endl;
        std::cerr << "Unable to draw file: " << filename << std::endl;
    }
    return false;
}

}}
