cmake_minimum_required(VERSION 3.7)
project(2D_BR)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 17)

include(contrib/contrib.cmake)
add_subdirectory(app)
add_subdirectory(server)