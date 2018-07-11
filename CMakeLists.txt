cmake_minimum_required(VERSION 3.5.0)

PROJECT(TiGa)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})

# We will replace lua with our own patched version
find_package(Lua)
include_directories(${LUA_INCLUDE_DIR})

#
find_package(SWIG)


include_directories(depend/include)
include_directories(depend/hge/include)
include_directories(simengine)


add_subdirectory(depend)

add_subdirectory(simengine)
include_directories(.)
# Disabling it for now. It is non-functional anyways
#add_subdirectory(game)
