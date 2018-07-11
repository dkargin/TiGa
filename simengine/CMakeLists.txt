message(STATUS "Searching for SDL2_TTF library --")
find_package(OpenGL REQUIRED)

# Find SDL
#INCLUDE(FindPkgConfig)

message(STATUS "Searching for SDL2 library --")
find_package(SDL2)
if(NOT SDL2_FOUND)
    MESSAGE(FATAL_ERROR "This project requires SDL2 to be installed to be compiled in graphical mode. Please install the SDL2 development libraries, or try compiling without the -DTILES=1 for a text only compilation.\nSee INSTALL file for details and more info\n" )
endif(NOT SDL2_FOUND)

message(STATUS "Searching for SDL2_TTF library --")
find_package(SDL2_ttf)
if(NOT SDL2_TTF_FOUND)
	MESSAGE(FATAL_ERROR "This project requires SDL2_ttf to be installed to be compiled in graphical mode. Please install the SDL2_ttf development libraries, or try compiling without the -DTILES=1 for a text only compilation.\nSee INSTALL file for details and moreinfo\n")
endif(NOT SDL2_TTF_FOUND)

message(STATUS "Searching for SDL2_image library --\n")
find_package(SDL2_image)
if(NOT SDL2_IMAGE_FOUND)
	MESSAGE(FATAL_ERROR "This project requires SDL2_image to be installed to be compiled in graphical mode. Please install the SDL2_image development libraries, or try compiling without the -DTILES=1 for a text only compilation.\nSee INSTALL file for details and more info\n")
endif(NOT SDL2_IMAGE_FOUND)


include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})

add_library(fx
	fx/rect.cpp
	fx/spritedata.cpp
	fx/fxentity.cpp
	fx/fxanimation.cpp
	fx/fxsound.cpp
	fx/fxsprite.cpp

	fx/manager.cpp
	fx/renderqueue.cpp
	fx/rendercontext.cpp
	fx/texture_manager.cpp
	fx/soundmanager.cpp
)

target_link_libraries(fx ${OPENGL_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_library(gui
	gui/guibase.cpp
	gui/guicontrols.cpp)

add_subdirectory(sim)

add_library(simengine application.cpp)

target_link_libraries(simengine fx gui sim ${SDL2_LIBRARIES})

add_subdirectory(examples)
