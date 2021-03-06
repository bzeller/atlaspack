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

#include <AtlasPack/atlaspack_global.h>
#include <AtlasPack/Backend>
#include <AtlasPack/PaintDevice>

namespace AtlasPack {
namespace Backends {

class ATLASPACK_EXPORT MagickBackend : public AtlasPack::Backend
{
    public:
        MagickBackend();

        // Backend interface
        virtual bool supportsImageType (const std::string &extension) const;
        std::shared_ptr<AtlasPack::PaintDevice> createPaintDevice(const AtlasPack::Size &reserveSize) const;
        AtlasPack::Image readImageInformation(const std::string &path) const;
};


class MagickPaintDevicePrivate;
class ATLASPACK_EXPORT MagickPaintDevice : public AtlasPack::PaintDevice
{
    public:
        MagickPaintDevice(const AtlasPack::Size &reserveSize);
        virtual ~MagickPaintDevice();

        // PaintDevice interface
        bool paintImageFromFile(AtlasPack::Pos topleft, std::string filename) override;
        bool exportToFile (std::string filename) override;

    private:
        MagickPaintDevicePrivate *p = nullptr;
};

}}
