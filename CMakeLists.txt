cmake_minimum_required(VERSION 3.1.0)

PROJECT(TiGa)

set (CMAKE_CXX_STANDARD 11)

include_directories(depend/include)
include_directories(HGETools)


add_subdirectory(depend)
add_subdirectory(game)