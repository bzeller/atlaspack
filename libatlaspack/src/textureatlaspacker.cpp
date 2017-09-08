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
#include <AtlasPack/TextureAtlasPacker>
#include <AtlasPack/textureatlas_p.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace fs =  boost::filesystem;

namespace AtlasPack {

struct Node {

    Node (Rect _rect = Rect())
        : rect(_rect){}

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Rect rect;
    Image img;
};

class TextureAtlasPackerPrivate {
    public:

    Node *insertImage (const Image &img, Node *node);
    bool collectNodes(TextureAtlasPrivate *atlas, std::shared_ptr<PaintDevice> painter, std::basic_ostream<char> *descStr, Node *node, std::string *err = nullptr);

    Node m_root;
};

/**
 * @brief TextureAtlasPrivate::insertImage
 * Algorithm based on lightmap packing found on http://blackpawn.com/texts/lightmaps/default.html
 * \returns the Node the Image was inserted into, nullptr if not enough space is available
 */
Node *TextureAtlasPackerPrivate::insertImage(const Image &img, Node *node)
{
    //if we have children, we are not a leaf
    if (node->left || node->right) {
        //first inside left:
        Node *newNode = insertImage(img, node->left.get());
        if (newNode)
            return newNode;

        //no space in left, insert right
        return insertImage(img, node->right.get());
    } else {
        //this path is entered if we found a leaf node

        //first check if the space is already filled
        if (node->img.isValid())
            return nullptr;

        Pos  nodePos  = node->rect.topLeft;
        Size nodeSize = node->rect.size;

        //check if there is enough room
        if (nodeSize.height < img.height()
                || nodeSize.width < img.width()) {
            //node too small
            return nullptr;
        }

        //check if we found a perfect fit
        if (nodeSize.height == img.height()
                && nodeSize.width == img.width()) {
            //perfect fit, store the image
            node->img = img;
            return node;
        }

        //At this poing the node is splitted up
        //we will split in a way that we always end up with the biggest possible
        //empty rectangle
        size_t remainWidth  = nodeSize.width - img.width();
        size_t remainHeight = nodeSize.height - img.height();

        if (remainWidth > remainHeight) {
            node->left  = std::make_shared<Node>(Rect(nodePos,
                                                      Size(img.width(), nodeSize.height)));

            node->right = std::make_shared<Node>(Rect(Pos(nodePos.x+img.width(), nodePos.y),
                                                      Size(nodeSize.width - img.width(), nodeSize.height)));
        } else {
            node->left  = std::make_shared<Node>(Rect(nodePos,
                                                      Size(nodeSize.width, img.height())));

            node->right = std::make_shared<Node>(Rect(Pos(nodePos.x, nodePos.y + img.height()),
                                                      Size(nodeSize.width, nodeSize.height - img.height())));
        }

        //now insert into leftmost Node
        return insertImage(img, node->left.get());
    }
}

bool TextureAtlasPackerPrivate::collectNodes(TextureAtlasPrivate *atlas, std::shared_ptr<PaintDevice> painter, std::basic_ostream<char> *descStr, Node *node, std::string *err)
{
    bool collected = false;
    if(node->img.isValid()) {
        collected = true;

        Texture t(node->rect.topLeft, node->img);
        atlas->m_textures[node->img.path()] = t;

        (*descStr) << node->img.path() <<","
                   << t.pos.x<<","
                   << t.pos.y<<","
                   << t.image.width()<<","
                   << t.image.height()<<"\n";

        if(!painter->paintImageFromFile(node->rect.topLeft, node->img.path())) {
            if (err) {
                *err = "Failed to paint image: "+node->img.path();
            }
            return false;
        }
    }

    if(node->left) {
        if(collected) std::cerr<<"Node has leafs AND image?"<<std::endl;
        if (!collectNodes(atlas, painter, descStr, node->left.get(), err))
            return false;
    }

    if(node->right){
        if(collected) std::cerr<<"Node has leafs AND image?"<<std::endl;
        if (!collectNodes(atlas, painter, descStr, node->right.get(), err))
            return false;
    }

    return true;
}

TextureAtlasPacker::TextureAtlasPacker(Size atlasSize)
    : p(new TextureAtlasPackerPrivate())
{
    p->m_root.rect = Rect(Pos(0,0), atlasSize);
}

TextureAtlasPacker::~TextureAtlasPacker()
{
    if (p) delete p;
}

Size TextureAtlasPacker::size() const
{
    return p->m_root.rect.size;
}

bool TextureAtlasPacker::insertImage(const Image &img)
{
    return p->insertImage(img, &p->m_root) != nullptr;
}

TextureAtlas TextureAtlasPacker::compile(const std::string &basePath, Backend *backend, std::string *error) const
{

    try {

        fs::path descFileName(basePath + ".atlas");
        fs::path textureFile(basePath + ".png");

        if (!fs::exists(descFileName.parent_path())
                || !fs::is_directory(descFileName.parent_path())) {
            if (error)
                *error = "Basepath is not a directory or does not exist";
            return TextureAtlas();
        }

#if 0
        if (fs::exists(descFileName)) {
            if (error)
                *error = "Atlas file exists already";
            return TextureAtlas();
        }

        if (fs::exists(textureFile)) {
            if (error)
                *error = "Texture file exists already";
            return TextureAtlas();
        }
#endif

        std::ofstream descFile(descFileName.string(), std::ios::trunc | std::ios::out);
        if(!descFile.is_open()) {
            if (error)
                *error = "Could not create atlas index file";
            return TextureAtlas();
        }

        auto painter = backend->createPaintDevice(p->m_root.rect.size);

        std::unique_ptr<TextureAtlasPrivate> priv = std::make_unique<TextureAtlasPrivate>();

        if(!p->collectNodes(priv.get(), painter, &descFile, &p->m_root, error))
            return TextureAtlas();

        if(!painter->exportToFile(textureFile.string())) {
            if (error) *error = "Failed to export Texture to file";
            return TextureAtlas();
        }

        descFile.close();
        return TextureAtlas(priv.release());


    } catch (...) {

    }

    return TextureAtlas();
}



}
