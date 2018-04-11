find_package(OpenGL REQUIRED)

# Find SDL
#INCLUDE(FindPkgConfig)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${OPENGL_INCLUDE_DIR})

add_library(fx

	fx/fxentity.cpp
	fx/fxanimation.cpp
	fx/fxsound.cpp
	fx/fxsprite.cpp

	fx/manager.cpp
	fx/rect.cpp
	fx/renderqueue.cpp
	fx/rendercontext.cpp
)

target_link_libraries(fx ${OPENGL_LIBRARIES})

add_library(gui
	gui/guibase.cpp
	gui/guicontrols.cpp)

add_subdirectory(sim)

add_library(simengine application.cpp)

target_link_libraries(simengine fx gui sim ${SDL2_LIBRARIES})

add_subdirectory(examples)
