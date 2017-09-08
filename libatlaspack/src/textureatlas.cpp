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

/**
 * @class TextureAtlas::TextureAtlas
 * Provides access to a previously created TextureAtlas
 * on disk using \a AtlasPack::PaintDevice to support loading
 * images from the texture atlas.
 * Use \a AtlasPack::TextureAtlasPacker to generate a new atlas
 * from files on disk.
 */


/**
 * @brief TextureAtlas::TextureAtlas
 * Privatre Constructor for use from the \a AtlasPack::TextureAtlasPacker
 * to when compiling a generated atlas.
 */
TextureAtlas::TextureAtlas(TextureAtlasPrivate *priv)
    : p(priv)
{

}

/**
 * @brief TextureAtlas::TextureAtlas
 * Generates a invalid TextureAtlas.
 */
TextureAtlas::TextureAtlas()
    : p(new TextureAtlasPrivate())
{
    p->m_valid = false;
}

/**
 * @brief TextureAtlas::TextureAtlas
 * Copy constructor to support copying of the TextureAtlas type
 */
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

/**
 * @brief TextureAtlas::load
 * Loads a texture atlas from disk, the \a basepath expects a path and filename without
 * extension to a texture atlas description and png file.
 * \example atlas.load("/tmp/myatlas"); //will expect /tmp/myatlas.atlas and /tmp/myatlas.png to exist
 */
bool TextureAtlas::load(const std::string basePath, std::string *error)
{
    return false;
}

/**
 * @brief TextureAtlas::isValid
 * Checks if the current TextureAtlas is valid, returns false if not.
 */
bool TextureAtlas::isValid() const
{
    return p->m_valid;
}

/**
 * @brief TextureAtlas::contains
 * Checks if the image specified by \a imgName is part of the atlas cache, returns true if it is
 */
bool TextureAtlas::contains(const std::string &imgName) const
{
    return (p->m_textures.find(imgName) != p->m_textures.end());
}

/**
 * @brief TextureAtlas::loadImage
 * Loads a image from the texture atlas specified by \a imgName and uses the \a painter to draw
 * the image at \a pos offset.
 */
bool TextureAtlas::loadImage(const std::string &imgName, PaintDevice *painter, Pos &targetPos)
{
    return false;
}

/**
 * @brief TextureAtlas::count
 * Returns the number of elements in the texture atlas
 */
size_t TextureAtlas::count() const
{
    return p->m_textures.size();
}

} // namespace AtlasPack
