set(HEADERS main_window.hpp)
set(SOURCES main_window.cpp)

add_library(sp_ui STATIC ${HEADERS} ${SOURCES})
add_library(sp::ui ALIAS sp_ui)
target_include_directories(sp_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Qt6 REQUIRED COMPONENTS Widgets)

target_link_libraries(sp_ui PUBLIC Qt::Widgets sp::opengl qcodeeditor qspdlog::lib
                                   qspdlog::interface)
