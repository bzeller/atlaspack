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
#include <AtlasPack/TextureAtlas>
#include <AtlasPack/JobQueue>

#include <AtlasPack/Backends/MagickBackend>

#include <iostream>
#include <thread>
#include <future>
#include <functional>
#include <utility>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>



namespace po = boost::program_options;
namespace fs = boost::filesystem;

/*
 * Returns the help text
 * \note Will contain a empty string if called before parseCommandLine
 */
static std::string &helpText () {
    static std::string text;
    return text;
}

/*
 * Prints the help to stdout
 * \note Will contain a empty string if called before parseCommandLine
 */
static void showHelp () {
    std::cerr<<helpText()<<std::endl;
}

/*
 * Collects all files that the backend supports in \a readDir.
 * If \a recursive is true all subdirectories will be searched as well
 */
static std::vector<AtlasPack::Image> collectImageFiles (AtlasPack::Backend *backend, const fs::path &readDir, bool recursive = false)
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

            if (!backend->supportsImageType(fs::extension(entry)))
                continue;

            AtlasPack::Image img = backend->readImageInformation(entry.path().string());
            if (!img.isValid()) {
                std::cerr << "Error when trying to load "<<entry.path().string()<<" skipping file."<<std::endl;
                continue;
            }

            //std::cout<<"Found Image: "<<img.path()<<std::endl;
            result.push_back(img);
        }

    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Error while reading the input directory: "<<ex.what() << std::endl;
    }

    return result;
}

/*
 * Builds the commandline parameters and parses the arguments. Returns \a true on success.
 */
static bool parseCommandline (int argc, char *argv[], po::variables_map &vm) {
    //Initialize the boost commandline parser with possible options
    po::options_description desc("General");
    desc.add_options()
            ("help,h", "Show this help message.");

    //split the arguments in pack and extract groups so the help is easier to read
    po::options_description descPack("pack");
    descPack.add_options()
            ("recursive,r", "Search also subdirectories for images");

    //the following options will not be shown in help, this is required for positional arguments
    po::options_description hiddenOptions("Hidden");
    hiddenOptions.add_options()
            ("input-or-output-file", po::value<std::string>(), "")
            ("atlasBaseName",  po::value<std::string>(), "Path and basename where the texture atlas should be placed");

    // Declare an options description instance which will include
    // all the options
    po::options_description allOptions("Allowed options");
    allOptions.add(desc).add(descPack).add(hiddenOptions);

    //the positional argument either represents the directory we want to pack or the filename we want to extract from the atlas
    po::positional_options_description posArgs;
    posArgs.add("input-or-output-file", 1);
    posArgs.add("atlasBaseName", 1);


    //generate a help message

    // Declare an options description instance which will only include arguments
    // we want to be visible
    po::options_description visibleOptions("Usage:\n"
                                           "  atlasbuilder [options] input_directory [output_filename]\n\n"
                                           "Example:"
                                           "\n  atlasbuilder /tmp/directory_with_files /tmp/MyAtlas\n");
    visibleOptions.add(desc).add(descPack);
    std::stringstream helpStr;
    helpStr << visibleOptions;
    helpText() = helpStr.str();

    //now parse the commandline, try to catch errors as good as possible
    try {
        po::store(po::command_line_parser(argc, argv).
                  options(allOptions).positional(posArgs).run(), vm);
        po::notify(vm);
    } catch (po::error &err) {
        //what will contain a user friendly error message, so we just can show that directly
        std::cout << "Error: " << err.what() << "\n\n";
        showHelp();
        return false;
    } catch (...) {
        std::cout << "Catched an unknown exception from the commandline parser, please check your options.\n\n";
        showHelp();
        return false;
    }

    //help was asked, lets show it and return a error
    if (vm.count("help")) {
        showHelp();
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{

    po::variables_map vm;
    if (!parseCommandline(argc, argv, vm)) {
        return 1;
    }

    //initialize the backend, this could be extended to load automatically
    //from plugins
    AtlasPack::Backends::MagickBackend backend;

    if (vm.count("input-or-output-file") != 1) {
        std::cerr << "Input directory was not specified."<<std::endl;
        showHelp();
        return 1;
    }

    //if no name is given the atlas is stored into the current working directory, and named output
    fs::path outputFileName;
    if (vm.count("atlasBaseName") == 0) {
        outputFileName = fs::current_path().append("output");
    } else {
        outputFileName = vm["atlasBaseName"].as<std::string>();
    }

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
        std::cout << "Starting to collect files"<<std::endl;
        images = collectImageFiles(&backend, readDir, vm.count("recursive") > 0);
        std::cout << "Collected "<<images.size()<<" files."<<std::endl;

    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Error while reading the input directory: "<<ex.what() << std::endl;
        return 1;
    }

    if (images.size() > 0) {

        AtlasPack::JobQueue<std::shared_ptr<AtlasPack::TextureAtlasPacker> > jobQueue;

        auto packer = [](const AtlasPack::Size &s, const std::vector<AtlasPack::Image> &images) {
            std::shared_ptr<AtlasPack::TextureAtlasPacker> result = std::make_shared<AtlasPack::TextureAtlasPacker>(s);
            for (const auto &img : images) {
                if (!result->insertImage(img)) {
                    return std::shared_ptr<AtlasPack::TextureAtlasPacker>();
                }
            }
            return result;
        };


        unsigned int cores = jobQueue.maxJobs();

        //first find a atlas that fits all, incremement by 1000
        size_t lastSize = 1000;
        size_t increment = 100;

        std::cout<<"Using "<<cores<<" cores to calculate Atlas"<<std::endl;

        std::shared_ptr<AtlasPack::TextureAtlasPacker> lastPossibleAtlas;

        while (!lastPossibleAtlas) {

            std::vector<std::future<std::shared_ptr<AtlasPack::TextureAtlasPacker> > > taskResults;

            for (unsigned int i = 0; i < cores; i++) {
                auto fun = std::bind(packer, AtlasPack::Size(lastSize, lastSize), images);
                taskResults.push_back(jobQueue.addTask(fun));
                lastSize += increment;
            }

            //wait until all tasks are finished
            jobQueue.waitForAllRunningTasks();

            //now find a Atlas that fits all images
            for (auto &currJob : taskResults ) {
                //if we get a Atlas we found one that fits
                lastPossibleAtlas = currJob.get();
                if (lastPossibleAtlas)
                    break;
            }
        }

        if (lastPossibleAtlas) {
            std::cout<<"Found a atlas that can contain all: "<<lastPossibleAtlas->size().height<<std::endl;
            std::cout<<"Trying to shrink Atlas size"<<std::endl;

            bool canGoOn = true;
            size_t decrement = 1;
            lastSize  = lastPossibleAtlas->size().height;
            while (canGoOn) {

                std::vector<std::future<std::shared_ptr<AtlasPack::TextureAtlasPacker> > > taskResults;

                for (unsigned int i = 0; i < cores; i++) {

                    //make sure we do not overflow
                    if(decrement > lastSize)
                        break;

                    AtlasPack::Size mySize = AtlasPack::Size(lastSize - decrement, lastSize - decrement);

                    auto fun = std::bind(packer, mySize, images);
                    taskResults.push_back(jobQueue.addTask(fun));

                    lastSize -= decrement;
                }

                //wait for all tasks to be finished
                jobQueue.waitForAllRunningTasks();

                for (auto &currJob : taskResults ) {
                    auto result = currJob.get();
                    if (result)
                        lastPossibleAtlas = result;
                    else {
                        //tell the loop we found a Atlas
                        canGoOn = false;
                        break;
                    }
                }
            }

            std::cout<<"Final Atlas size: "<<lastPossibleAtlas->size().height<<std::endl;
            std::cout<<"Compiling Atlas, this can take a lot of time ....."<<std::endl;

            std::string err;
            AtlasPack::TextureAtlas atlas = lastPossibleAtlas->compile(outputFileName.string(), &backend, &err);

            if(atlas.isValid()) {
                std::cout<<"Created a Atlas with "<<atlas.count()<<" Images from a List of "<<images.size()<<" Images."<<std::endl;
                return 0;
            } else {
                std::cout<<"Failed to create Atlas, error was: "<<err<<std::endl;
                return 1;
            }
        }
    }


    return 0;
}
