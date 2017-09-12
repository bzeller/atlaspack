# atlaspack
Small tool to pack a texture atlas from a directory of images

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
6. git clone atlaspack into c:\dev\project\atlaspack
7. Open the Visual Studio command prompt
8. Create Visual Studio project files with:
   cd c:\dev\project
   mkdir c:\dev\project\build-vs
   cd build-vs
   C:\dev\cmake\bin\cmake.exe -DBOOST_ROOT=C:\dev\boost -DBOOST_LIBRARYPATH=C:\dev\boost\stage\lib ../atlaspack

After the last step there should be atlaspack.sln created in the build-vs directory that can be opened and built inside
Visual Studio 2015. Please make ABSOLUTELY sure to use the release build configuration otherwise binary incompatibilities
would cause problems in the application.
