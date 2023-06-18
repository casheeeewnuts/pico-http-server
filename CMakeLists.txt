cmake_minimum_required(VERSION 3.25)
project(pico_http_server C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)

include_directories(${GLIB_INCLUDE_DIRS})
add_executable(pico_http_server main.c http/http_request.h http/http_request.c http/http_server.c http/http_response.c http/util/path.c http/util/path.h http/http_header.c http/http_header.h)
target_link_libraries(pico_http_server ${GLIB_LIBRARIES})
