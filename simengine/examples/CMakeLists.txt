
add_executable(minimal_gl_app
	sdl2_minimal_gl_app.cpp)

target_link_libraries(minimal_gl_app ${SDL2_LIBRARIES})
target_link_libraries(minimal_gl_app ${OPENGL_LIBRARIES})
