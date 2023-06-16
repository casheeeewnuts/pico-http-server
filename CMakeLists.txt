cmake_minimum_required(VERSION 3.25)
project(pico_http_server C)

set(CMAKE_C_STANDARD 11)

add_executable(pico_http_server main.c http/http_request.h http/http_request.c)
