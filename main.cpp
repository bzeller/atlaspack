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

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <AtlasPack/TextureAtlas>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

std::vector<AtlasPack::Image> collectImageFiles (AtlasPack::Backend *backend, const fs::path &readDir, bool recursive = false)
{
    std::vector<AtlasPack::Image> result;

    try {
        if (!fs::exists(readDir)) {
            //Input directory does not exist.
            return result;
        }

        if (!fs::is_directory(readDir)) {
            //Input path is not a directory.
            return result;
        }

        for (fs::directory_entry& entry : fs::directory_iterator(readDir)) {
            boost::system::error_code err;

            if (fs::is_directory(entry, err)) {
                if (err.value() != boost::system::errc::success) {
                    std::cerr << "Error when trying to stat "<<entry.path().string()<<" skipping entry."<<std::endl;
                    continue;
                }

                if (recursive) {
                    std::vector<AtlasPack::Image> subDirItems = collectImageFiles(backend, entry, true);
                    result.reserve(result.size() + subDirItems.size());
                    result.insert(result.end(), subDirItems.begin(), subDirItems.end());
                }
                continue;
            }

            if (!fs::is_regular_file(entry, err)) {
                if (err.value() != boost::system::errc::success) {
                    std::cerr << "Error when trying to stat "<<entry.path().string()<<" skipping entry."<<std::endl;
                }
                continue;
            }

            AtlasPack::Image img = backend->readImageInformation(entry.path().string());
            if (!img.isValid()) {
                std::cerr << "Error when trying to load "<<entry.path().string()<<" skipping file."<<std::endl;
                continue;
            }
            result.push_back(img);
        }

    } catch (const fs::filesystem_error& ex) {
      std::cerr << "Error while reading the input directory: "<<ex.what() << std::endl;
    }

    return result;
}

int main(int argc, char *argv[])
{
    //Initialize the boost commandline parser with possible options
    po::options_description desc("General");
    desc.add_options()
        ("help,h", "Show this help message.")
        ("mode,m", po::value<std::string>()->required(), "Mode of operation, can be pack or extract");

    //split the arguments in pack and extract groups so the help is easier to read
    po::options_description descPack("pack");
    descPack.add_options()
            ("atlasDimensions,a",  po::value<std::string>(), "\"Width x Height\" Force dimensions of the created Atlas.")
            ("atlasBaseName,b",  po::value<std::string>(), "Path and basename where the texture atlas should be placed")
            ("recursive,r", "Search also subdirectories for images");

    po::options_description descUnpack("extract");
    descUnpack.add_options()
            ("output,o",  po::value<std::string>(), "Path where the extracted image should be placed");

    //the following options will not be shown in help, this is required for positional arguments
    po::options_description hiddenOptions("Hidden");
    hiddenOptions.add_options()
        ("input-or-output-file", po::value<std::string>(), "");

    // Declare an options description instance which will include
    // all the options
    po::options_description allOptions("Allowed options");
    allOptions.add(desc).add(descPack).add(descUnpack).add(hiddenOptions);

    //the positional argument either represents the directory we want to pack or the filename we want to extract from the atlas
    po::positional_options_description posArgs;
    posArgs.add("input-or-output-file", 1);


    //generate a help message
    auto showHelp = [&]() {
        // Declare an options description instance which will only include arguments
        // we want to be visible
        po::options_description visibleOptions("Usage:\n"
                                               "  atlasbuilder --mode=pack [options] input_directory\n"
                                               "  atlasbuilder --mode=extract [options] filename_to_extract\n\n"
                                               "The atlasbuilder has two different modes, one for packing a directory\n"
                                               "and one for extracting a file from a texture atlas:\n"
                                               "\nBuild atlas:\n  atlasbuilder --mode=pack -o /tmp/atlas.jpg /tmp/directory_with_files\n"
                                               "\nExtract from atlas:\n  atlasbuilder --mode=extract -o /tmp/output.jpg /tmp/atlas.json\n\n");
        visibleOptions.add(desc).add(descPack).add(descUnpack);
        std::cout << visibleOptions << "\n";
    };


    //now parse the commandline, try to catch errors as good as possible
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).
                  options(allOptions).positional(posArgs).run(), vm);
        po::notify(vm);
    } catch (po::error &err) {
        //what will contain a user friendly error message, so we just can show that directly
        std::cout << "Error: " << err.what() << "\n\n";
        showHelp();
        return 1;
    } catch (...) {
        std::cout << "Catched an unknown exception from the commandline parser, please check your options.\n\n";
        showHelp();
        return 1;
    }

    //help was asked, lets show it and return a error
    if (vm.count("help")) {
        showHelp();
        return 1;
    }

    if (vm["mode"].as<std::string>() == "pack") {

        if (vm.count("input-or-output-file") != 1) {
            std::cerr << "Input directory was not specified."<<std::endl;
            showHelp();
            return 1;
        }

        //initialize the backend, this could be extended to load automatically
        //from plugins
        MagickBackend backend;

        std::vector<AtlasPack::Image> images;

        fs::path readDir(vm["input-or-output-file"].as<std::string>());
        try {
            if (!fs::exists(readDir)) {
                std::cerr << "Input directory does not exist."<<std::endl;
                return 1;
            }

            if (!fs::is_directory(readDir)) {
                std::cerr << "Input path is not a directory."<<std::endl;
                return 1;
            }
            images = collectImageFiles(&backend, readDir, vm.count("recursive") > 0);

            for (const AtlasPack::Image &currImg : images) {
                std::cout<<"Found Image: "<<currImg.path()<<std::endl;
            }
        } catch (const fs::filesystem_error& ex) {
          std::cerr << "Error while reading the input directory: "<<ex.what() << std::endl;
        }

        if (images.size() > 0) {
            AtlasPack::TextureAtlas atlasbuilder(AtlasPack::Size(4000,4000));
            std::random_shuffle(images.begin(), images.end());
            for (const auto &img : images) {
                if (!atlasbuilder.insertImage(img)) {
                    std::cerr << "Not enough space to insert image "<<img.path()<<std::endl;
                    return 1;
                } else {
                    std::cout << "Added image "<<img.path()<<std::endl;
                }
            }

            atlasbuilder.render(&backend);
        }

    } else if (vm["mode"].as<std::string>() == "extract") {
        std::cout << "Do extract";
    } else {
        std::cout << "Error: Unknown mode used, only pack and extract are supported.\n\n";
        showHelp();
        return 1;
    }


    return 0;
}
