add_executable(shader_playground main.cpp)

target_link_libraries(shader_playground PRIVATE sp::ui sp::opengl)
