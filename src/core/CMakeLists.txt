set(HEADERS logger.hpp)
set(SOURCES logger.cpp)

add_library(sp_core STATIC ${HEADERS} ${SOURCES})
add_library(sp::core ALIAS sp_core)
target_include_directories(sp_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(spdlog REQUIRED)

target_link_libraries(sp_core PUBLIC spdlog::spdlog)
