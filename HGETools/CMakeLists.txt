find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

add_library(fx

	src/fxentity.cpp
	src/fxanimation.cpp
	src/fxsound.cpp
	src/fxsprite.cpp

	src/guibase.cpp
	src/guicontrols.cpp
	src/manager.cpp
	src/rect.cpp
	src/renderqueue.cpp
)

target_link_libraries(fx ${OPENGL_LIBRARIES})