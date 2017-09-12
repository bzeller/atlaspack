cmake_minimum_required(VERSION 2.8)

project(atlaspack C CXX)

# enable C++14 features
if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "-std=gnu++14 ${CMAKE_CXX_FLAGS}")
  endif ()
else ()
  set (CMAKE_CXX_STANDARD 14)
endif ()

# require Boost libraries
if(MSVC)
    set(Boost_USE_STATIC_LIBS        ON) # only find static libs
    set(Boost_USE_MULTITHREADED      ON)
    set(Boost_USE_STATIC_RUNTIME    OFF)
else()
    set(Boost_USE_STATIC_LIBS       OFF)
    set(Boost_USE_MULTITHREADED      ON)
    set(Boost_USE_STATIC_RUNTIME    OFF)
endif()
find_package(Boost
  "1.58"
  REQUIRED
  program_options filesystem
)

#threading
find_package (Threads)

add_subdirectory(libatlaspack)

INCLUDE_DIRECTORIES(libatlaspack/include ${Boost_INCLUDE_DIRS})

set (SOURCES
    main.cpp
    )

add_executable(${PROJECT_NAME}-cli ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME}-cli Boost::program_options atlaspack Threads::Threads  ${Boost_LIBRARIES})
