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
#include <AtlasPack/JobQueue>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace fs =  boost::filesystem;

namespace AtlasPack {

/**
 * \internal
 * Represents a Node in the packing tree algorithm,
 * can contain either child nodes or a image. The rect
 * variable is always set.
 */
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
    bool collectNodes(TextureAtlasPrivate *atlas, std::shared_ptr<PaintDevice> painter, std::basic_ostream<char> *descStr,
                      Node *node, JobQueue<bool> *painterQueue, std::vector<std::future<bool> > &painterResults, std::string *err = nullptr);

    Node m_root;
};

/**
 * @brief TextureAtlasPrivate::insertImage
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

/**
 * @internal
 * @brief TextureAtlasPackerPrivate::collectNodes
 * Iterates over the full tree, filling the \a atlas and painting the images using the \a painter as well as writing
 * the image rectangle and filenmame into the output stream given by \a descStr.
 * If a error occurs and \a err is set, a error message is put there.
 */
bool TextureAtlasPackerPrivate::collectNodes(TextureAtlasPrivate *atlas, std::shared_ptr<PaintDevice> painter,
                                             std::basic_ostream<char> *descStr, Node *node,
                                             JobQueue<bool> *painterQueue, std::vector<std::future<bool>> &painterResults,
                                             std::string *err)
{
    bool collected = false;
    if(node->img.isValid()) {
        collected = true;

        // we found a Image node, lets fill the information into the given structures
        Texture t(node->rect.topLeft, node->img);
        atlas->m_textures[node->img.path()] = t;

        // Renders the node into the atlas image, called from a async thread
        auto fun = [](std::shared_ptr<PaintDevice> painter, Node *node){
            // paint the texture into the cache image
            if(!painter->paintImageFromFile(node->rect.topLeft, node->img.path())) {
                std::cout<<"Failed to paint image "<<node->img.path();
                return false;
            }
            return true;
        };

        // push the future results into a vector, so we can check if we had errors after all tasks are done
        painterResults.push_back(painterQueue->addTask(std::bind(fun, painter, node)));

        // the description file is written as a CSV file
        // @NOTE possible room for improvement, make the description file structure modular,
        // to make it easy to use another format
        (*descStr) << node->img.path() <<","
                   << t.pos.x<<","
                   << t.pos.y<<","
                   << t.image.width()<<","
                   << t.image.height()<<"\n";

    }

    if (collected && (node->left || node->right )) {
        //this should never happen, if it does at least print a warning about it
        if(collected) std::cerr<<"Node has leafs AND image?"<<std::endl;
    }

    //recursively iterate through the child nodes, start with the left node again
    if(node->left) {
        if (!collectNodes(atlas, painter, descStr, node->left.get(), painterQueue, painterResults, err))
            return false;
    }

    if(node->right){
        if (!collectNodes(atlas, painter, descStr, node->right.get(), painterQueue, painterResults, err))
            return false;
    }

    return true;
}

/**
 * @class TextureAtlasPacker::TextureAtlasPacker
 * Implements a packing algorithm to pack images into a bigger texture, called
 * a \a AtlasPack::TextureAtlas. This can speed up image loading in applications that make use
 * of a lot of small image files.
 *
 * This implementation makes use of the lightmap packing algorithm that can be found at http://blackpawn.com/texts/lightmaps/default.html
 */


TextureAtlasPacker::TextureAtlasPacker(Size atlasSize)
    : p(new TextureAtlasPackerPrivate())
{
    p->m_root.rect = Rect(Pos(0,0), atlasSize);
}

TextureAtlasPacker::~TextureAtlasPacker()
{
    if (p) delete p;
}

/*!
 * \brief TextureAtlasPacker::size
 * Returns the current geometrical size of the texture atlas.
 */
Size TextureAtlasPacker::size() const
{
    return p->m_root.rect.size;
}

/*!
 * \brief TextureAtlasPacker::insertImage
 * Tried to insert the \sa AtlasPack::Image given by \a img into the atlas.
 * The internal algorithm will split the atlas rectangle into smaller portions
 * until the image fits.
 * Returns \a true on success, or \a false in case the atlas does not have enough remaining space.
 */
bool TextureAtlasPacker::insertImage(const Image &img)
{
    return p->insertImage(img, &p->m_root) != nullptr;
}


/**
 * \brief TextureAtlasPacker::compile
 * Compiles the current in memory state of the TextureAtlas into a description and
 * image file and stores them on disk. Expects \a basePath to point at a user writeable directory,
 * the last part of \a basePath will be used to form the texture atlas description file and image file names.
 *
 * \note This can take a lot of time for a big list of images, however the implementation does run with multiple
 *       threads to speed the process up.
 */
TextureAtlas TextureAtlasPacker::compile(const std::string &basePath, Backend *backend, std::string *error) const
{

    try {

        //the basepath is used to create the filenames for the 2 output files
        fs::path descFileName(basePath + ".atlas");
        fs::path textureFile(basePath + ".png");

        JobQueue<bool> jobs;

        //check if the output directory exists
        if (!fs::exists(descFileName.parent_path())
                || !fs::is_directory(descFileName.parent_path())) {
            if (error)
                *error = "Basepath is not a directory or does not exist";
            return TextureAtlas();
        }

        //create atlas description text file
        std::ofstream descFile(descFileName.string(), std::ios::trunc | std::ios::out);
        if(!descFile.is_open()) {
            if (error)
                *error = "Could not create atlas index file";
            return TextureAtlas();
        }

        //get new painter instance from the backend
        auto painter = backend->createPaintDevice(p->m_root.rect.size);

        std::unique_ptr<TextureAtlasPrivate> priv = std::make_unique<TextureAtlasPrivate>();
        std::vector<std::future<bool> > paintResults;

        //recursively collect all nodes, write them to the desc file and give paint tasks to the
        //JobQueue to run asynchronously
        if(!p->collectNodes(priv.get(), painter, &descFile, &p->m_root, &jobs, paintResults, error))
            return TextureAtlas();

        //wait until all painters are done
        jobs.waitForAllRunningTasks();

        //check if we have errors in some of the painters, no need
        //to print which one here, because the painters will print a error message on their
        //own if required
        for (std::future<bool> &res : paintResults) {
            if (!res.get()) {
                std::cout<<"Some images failed to paint";
                return TextureAtlas();
            }
        }

        //finally save the result to a file
        if(!painter->exportToFile(textureFile.string())) {
            if (error) *error = "Failed to export Texture to file";
            return TextureAtlas();
        }

        descFile.close();
        return TextureAtlas(priv.release());

    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Filesystem error while compiling the texture atlas: "<<ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Catched an unknown exception when compiling the texture atlas."<< std::endl;
    }

    return TextureAtlas();
}



}
