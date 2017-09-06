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
#include <AtlasPack/TextureAtlas>
#include <iostream>

namespace AtlasPack {

struct Node {

    Node (Rect _rect = Rect())
        : rect(_rect){}

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Rect rect;
    const Image *img = nullptr;
};

class TextureAtlasPrivate {
    public:

    Node *insertImage (const Image &img, Node *node);
    void renderNode (std::shared_ptr<PaintDevice> painter, Node *node);

    Node m_root;
};

//Algorithm based on lightmap packing found on http://blackpawn.com/texts/lightmaps/default.html
Node *TextureAtlasPrivate::insertImage(const Image &img, Node *node)
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
        if (node->img)
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
            node->img = &img;
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

void TextureAtlasPrivate::renderNode(std::shared_ptr<PaintDevice> painter, Node *node)
{
    bool rendered = false;
    if(node->img) {
        rendered = true;
        painter->paintImageFromFile(node->rect.topLeft, node->img->path());
    }

    if(node->left) {
        if(rendered) std::cerr<<"Node has leafs AND image?"<<std::endl;
        renderNode(painter, node->left.get());
    }

    if(node->right){
        if(rendered) std::cerr<<"Node has leafs AND image?"<<std::endl;
        renderNode(painter, node->right.get());
    }
}

TextureAtlas::TextureAtlas(Size atlasSize)
    : p(new TextureAtlasPrivate())
{
    p->m_root.rect = Rect(Pos(0,0), atlasSize);
}

TextureAtlas::~TextureAtlas()
{
    if (p) delete p;
}

bool TextureAtlas::insertImage(const Image &img)
{
    return p->insertImage(img, &p->m_root) != nullptr;
}

bool TextureAtlas::render(Backend *backend)
{
    auto painter = backend->createPaintDevice(p->m_root.rect.size);
    p->renderNode(painter, &p->m_root);
    return painter->exportToFile("/tmp/mafirstmap.jpg");
}



}
