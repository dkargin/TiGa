find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

add_library(fx
	src/animation.cpp
	src/effect.cpp
	src/guibase.cpp
	src/guicontrols.cpp
	src/manager.cpp
	src/rect.cpp
	src/sound.cpp
	src/sprite.cpp
	
)

target_link_libraries(fx ${OPENGL_LIBRARIES})