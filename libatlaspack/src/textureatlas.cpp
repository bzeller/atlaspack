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

#include <AtlasPack/textureatlas_p.h>

namespace AtlasPack {


TextureAtlas::TextureAtlas(TextureAtlasPrivate *priv)
    : p(priv)
{

}

TextureAtlas::TextureAtlas()
    : p(new TextureAtlasPrivate())
{
    p->m_valid = false;
}

TextureAtlas::TextureAtlas(const TextureAtlas &other)
    : p(new TextureAtlasPrivate())
{
    *p = *other.p;
}

TextureAtlas::~TextureAtlas()
{
    if(p) delete p;
}

TextureAtlas &TextureAtlas::operator=(const TextureAtlas &other)
{
    *p = *other.p;
    return *this;
}

bool TextureAtlas::load(const std::string basePath, std::string *error)
{
    return false;
}

bool TextureAtlas::isValid() const
{
    return p->m_valid;
}

bool TextureAtlas::contains(const std::string &imgName) const
{
    return (p->m_textures.find(imgName) != p->m_textures.end());
}

bool TextureAtlas::loadImage(const std::string &imgName, PaintDevice *painter, Pos &targetPos)
{
    return false;
}

size_t TextureAtlas::count() const
{
    return p->m_textures.size();
}

} // namespace AtlasPack
