cmake_minimum_required(VERSION 3.0)

project(web-diff-files-tree C CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/src.wsjcpp/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${web-diff-files-tree_SOURCE_DIR})

# include header dirs
list (APPEND WSJCPP_INCLUDE_DIRS "src")
list (APPEND WSJCPP_INCLUDE_DIRS "src/database")
list (APPEND WSJCPP_INCLUDE_DIRS "src/server")

list (APPEND WSJCPP_SOURCES "src/main.cpp")

#### BEGIN_WSJCPP_APPEND
#### END_WSJCPP_APPEND

# database
list (APPEND WSJCPP_SOURCES "./src/database/models_diff.cpp")
list (APPEND WSJCPP_SOURCES "./src/database/models_basic.cpp")
list (APPEND WSJCPP_SOURCES "./src/database/models_cache.cpp")
list (APPEND WSJCPP_SOURCES "./src/database/mysql_storage.cpp")

# server
list (APPEND WSJCPP_SOURCES "./src/server/webdiff_http_server.cpp")
list (APPEND WSJCPP_SOURCES "./src/server/webdiff_ws_server.cpp")
list (APPEND WSJCPP_SOURCES "./src/server/webdiff_server.cpp")
list (APPEND WSJCPP_SOURCES "./src/server/player_context.cpp")

# libhv
SET(BUILD_EXAMPLES OFF CACHE BOOL "build examples")
add_subdirectory(src-libhv)

list (APPEND WSJCPP_LIBRARIES hv_static)
list (APPEND WSJCPP_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/src-libhv/include/hv")

############################
##### mysql 
# Find and make sure the system have the header file
find_path(MYSQL_HEADER mysql/mysql.h)
if(MYSQL_HEADER STREQUAL "MYSQL_HEADER-NOTFOUND")
    message(FATAL_ERROR "Could not find the mysql/mysql.h header file: (ubuntu: apt install libmysqlclient-dev) or (debian: apt install default-libmariadbclient-dev and link: ln -s /usr/lib/x86_64-linux-gnu/pkgconfig/mariadb.pc /usr/lib/x86_64-linux-gnu/pkgconfig/mysqlclient.pc)")
endif()

include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

list (APPEND WSJCPP_INCLUDE_DIRS ${LIBMYSQLCLIENT_LIBRARY_DIRS})
list (APPEND WSJCPP_LIBRARIES ${LIBMYSQLCLIENT_LIBRARIES})
#####
############################


include_directories(${WSJCPP_INCLUDE_DIRS})

add_executable (web-diff-files-tree ${WSJCPP_SOURCES})

target_link_libraries(web-diff-files-tree ${WSJCPP_LIBRARIES})

install(
    TARGETS
        web-diff-files-tree
    RUNTIME DESTINATION
        /usr/bin
)


