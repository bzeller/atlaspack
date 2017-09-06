cmake_minimum_required(VERSION 2.8)

project(atlaspack)

# enable C++14 features
if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "-std=gnu++14 ${CMAKE_CXX_FLAGS}")
  endif ()
else ()
  set (CMAKE_CXX_STANDARD 14)
endif ()

# require Boost libraries
find_package(Boost
  "1.58"
  REQUIRED
  program_options
)

# image magick for the backend implementation
find_package(ImageMagick
    REQUIRED
    COMPONENTS Magick++
)

add_subdirectory(libatlaspack)

INCLUDE_DIRECTORIES(libatlaspack/include ${ImageMagick_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

set (HEADERS
    magickbackend.h
    )

set (SOURCES
    magickbackend.cpp
    main.cpp
    )

add_executable(${PROJECT_NAME}-cli ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME}-cli Boost::program_options libatlaspack ${ImageMagick_LIBRARIES})
