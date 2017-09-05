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

#include <iostream>
#include <boost/program_options.hpp>


namespace po = boost::program_options;

//move to library
struct Dimension {
    int width  = 0;
    int height = 0;
};

int doPack   (const std::string inputDirectory, const std::string basePath, const Dimension &dim, bool recursive = false);
int doExtract (const std::string filename, const std::string outputFile, const std::string atlasPath);

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
        std::cout << "Do package";
    } else if (vm["mode"].as<std::string>() == "extract") {
        std::cout << "Do extract";
    } else {
        std::cout << "Error: Unknown mode used, only pack and extract are supported.\n\n";
        showHelp();
        return 1;
    }


    return 0;
}
