cmake_minimum_required(VERSION 2.8)

project(atlasbuilder)

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

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} Boost::program_options)
