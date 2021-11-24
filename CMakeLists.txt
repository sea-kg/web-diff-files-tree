cmake_minimum_required(VERSION 3.0)

project(web-diff-files-tree C CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/src.wsjcpp/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${web-diff-files-tree_SOURCE_DIR})

# include header dirs
list (APPEND WSJCPP_INCLUDE_DIRS "src")
list (APPEND WSJCPP_INCLUDE_DIRS "src/server")

list (APPEND WSJCPP_SOURCES "src/main.cpp")

#### BEGIN_WSJCPP_APPEND
#### END_WSJCPP_APPEND

# server
list (APPEND WSJCPP_SOURCES "./src/server/webdiff_http_server.cpp")
list (APPEND WSJCPP_SOURCES "./src/server/webdiff_ws_server.cpp")
list (APPEND WSJCPP_SOURCES "./src/server/webdiff_server.cpp")
list (APPEND WSJCPP_SOURCES "./src/server/player_context.cpp")

# libhv
add_subdirectory(src-libhv)
list (APPEND WSJCPP_LIBRARIES hv_static)
list (APPEND WSJCPP_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/src-libhv/include/hv")

include_directories(${WSJCPP_INCLUDE_DIRS})

add_executable (web-diff-files-tree ${WSJCPP_SOURCES})

target_link_libraries(web-diff-files-tree ${WSJCPP_LIBRARIES})

install(
    TARGETS
        web-diff-files-tree
    RUNTIME DESTINATION
        /usr/bin
)


