# atlaspack
Small tool to pack a texture atlas from a directory of images.

The tool is implemented using C++11 features as well as parts from the Boost library.
By splitting the source code into a library and executable part it is possible to reuse
the logic inside any application.

Also the libatlaspack library offers possibility to implement a different image processing
backend. By implementing the Backend and PaintDevice interfaces it is possible to completely
switch those out. One example Backend is provided which is using ImageMagick++ to implement
the image processing.

In order to speed up image processing and creation of the image, libatlaspack is using
concurrent tasks, the JobQueue is a reuseable template class that can run any callable inside
a seperate thread, returning the result as a std::future.

Possible known issues are:
---------------------------
- The error handling for the commandline parameter can be made more intelligent
- The texture packing algorithm is not optimal in all cases, depending on the order and sizes
  of the input images gaps may be visible
- The implementation to extract a image from the texture atlas is missing
- Plugin loading could be implemented to make the use of Backends more flexible


Usage:
  atlaspack-cli [options] input_directory [output_filename]

Example:
  atlasbuilder /tmp/directory_with_files /tmp/MyAtlas

General:
  -h [ --help ]          Show this help message.

pack:
  -r [ --recursive ]     Search also subdirectories for images


Calling the tool as in the example: "atlaspack-cli /tmp/directory_with_files /tmp/MyAtlas" will
generate a /tmp/MyAtlas.atlas and /tmp/MyAtlas.png. The .atlas file contains the texture description
with image name, x and y offset as well as width and height of the image in CSV format.

If no output filename is given, the tool by default will generate a atlas in the working directory with
output.atlas and output.png filenames.

Third-Party dependencies:
--------------------------
Boost 1.5.8
ImageMagick 7.0.7-1


Build on Ubuntu 16.04 LTS
--------------------------------------------------------------------

# Install dependencies:
sudo apt install build-essential git libboost-all-dev libmagick++-dev cmake

# build
git clone https://github.com/bzeller/atlaspack.git
cd atlaspack
mkdir build && cd build
cmake ..
make
./atlaspack-cli --help


Build on Windows 7 32bit using Visual Studio 2015 Express:
--------------------------------------------------------------------

Dependencies:
- Visual Studio Express 2015
- Boost 1.5.8
- ImageMagick 7.0.7-1-Q8-x86

1. Create a dedicated project directory. For example C:\dev
2. Download Boost sources v 1.5.8 and extract them to C:\dev\boost
3. Build Boost with:
   cd c:\dev\boost
   bootstrap
   .\b2
   .\b2 program_options filesystem system
4. Download ImageMagick , the one tested with was https://www.imagemagick.org/download/binaries/ImageMagick-7.0.7-1-Q8-x86-dll.exe
5. Install ImageMagick to c:\dev
6. Download CMake from https://cmake.org/files/v3.8/cmake-3.8.2-win32-x86.msi and install into C:\dev\cmake
7. git clone atlaspack into c:\dev\project\atlaspack
8. Open the Visual Studio command prompt
9. Create Visual Studio project files with:
   cd c:\dev\project
   mkdir c:\dev\project\build-vs
   cd build-vs
   C:\dev\cmake\bin\cmake.exe -DBOOST_ROOT=C:\dev\boost -DBOOST_LIBRARYPATH=C:\dev\boost\stage\lib ../atlaspack

After the last step there should be atlaspack.sln created in the build-vs directory that can be opened and built inside
Visual Studio 2015. Please make ABSOLUTELY sure to use the release build configuration otherwise binary incompatibilities
would cause problems in the application.

In case the application exits with a "No space left on device error", the output image was too big, so the ImageMagick backend
rejected to create it. This behaviour could be changed by implementing a different backend that supports the creation of extremely
large files.
