
# Reference SDL application
add_executable(minimal_gl_app sdl2_minimal_gl_app.cpp)
target_link_libraries(minimal_gl_app ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

# Test application for simengine
add_executable(test_application test_application.cpp)
target_link_libraries(test_application simengine)
