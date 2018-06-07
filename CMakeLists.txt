cmake_minimum_required(VERSION 3.9)
project(meshell)

set(CMAKE_CXX_STANDARD 17)
include_directories(/usr/include/readline)



add_executable(myshell meshell.cpp meshell.h)

target_link_libraries(myshell readline)
add_executable(myls myls.cpp myls.h)