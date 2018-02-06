cmake_minimum_required(VERSION 3.1.0)

PROJECT(TiGa)

set (CMAKE_CXX_STANDARD 11)

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
add_subdirectory(game)