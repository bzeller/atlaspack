cmake_minimum_required(VERSION 2.8)

project(libatlaspack)

set (HEADERS
    include/AtlasPack/dimension.h
    include/AtlasPack/paintdevice.h
    include/AtlasPack/Dimension
    include/AtlasPack/PaintDevice
    include/AtlasPack/TextureAtlasPacker
    include/AtlasPack/textureatlaspacker.h
    include/AtlasPack/TextureAtlas
    include/AtlasPack/textureatlas.h
    include/AtlasPack/Image
    include/AtlasPack/image.h
    include/AtlasPack/Backend
    include/AtlasPack/backend.h
    )

set (SOURCES
    src/textureatlaspacker.cpp
    src/textureatlas.cpp
    src/backend.cpp
    src/image.cpp
    )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${ImageMagick_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME})
