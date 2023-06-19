set(HEADERS view.hpp texture.hpp)
set(SOURCES view.cpp texture.cpp)

add_library(sp_opengl STATIC ${HEADERS} ${SOURCES})
add_library(sp::opengl ALIAS sp_opengl)
target_include_directories(sp_opengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)

target_link_libraries(sp_opengl PUBLIC Qt::OpenGLWidgets)
