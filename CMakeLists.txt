cmake_minimum_required(VERSION 3.10)

project(shader_playground)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(thirdparty/qspdlog)
add_subdirectory(thirdparty/qcodeeditor)

add_subdirectory(src)
