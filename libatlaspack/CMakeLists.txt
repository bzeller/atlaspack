cmake_minimum_required(VERSION 2.8)

project(atlaspack C CXX)

# image magick for the backend implementation
find_package(ImageMagick
    REQUIRED
    COMPONENTS Magick++
)

# require Boost libraries
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost
  "1.58"
  REQUIRED
  program_options filesystem system
)

# add Visual studio specific defines
if(MSVC) 
add_definitions(
    -D_VISUALC_
    -DNeedFunctionPrototypes
    -D_DLL
    -D_MAGICKMOD_
)
endif()

add_definitions(
    -DATLASPACK_LIBRARY
    )

set (HEADERS
    include/AtlasPack/dimension.h
    include/AtlasPack/paintdevice.h
    include/AtlasPack/Dimension
    include/AtlasPack/PaintDevice
    include/AtlasPack/TextureAtlasPacker
    include/AtlasPack/textureatlaspacker.h
    include/AtlasPack/TextureAtlas
    include/AtlasPack/textureatlas.h
    include/AtlasPack/textureatlas_p.h
    include/AtlasPack/Image
    include/AtlasPack/image.h
    include/AtlasPack/Backend
    include/AtlasPack/backend.h
    include/AtlasPack/JobQueue
    include/AtlasPack/jobqueue.h
    include/AtlasPack/atlaspack_global.h
    include/AtlasPack/Backends/MagickBackend
    include/AtlasPack/Backends/magickbackend.h
    )

set (SOURCES
    src/textureatlaspacker.cpp
    src/textureatlas.cpp
    src/paintdevice.cpp
    src/backend.cpp
    src/image.cpp
    src/backends/magickbackend.cpp
    )

message("INCLUDE DIR ${ImageMagick_INCLUDE_DIRS}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${ImageMagick_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME}  ${ImageMagick_LIBRARIES}  ${Boost_LIBRARIES})
