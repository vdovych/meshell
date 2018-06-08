cmake_minimum_required(VERSION 3.9)
project(meshell)

set(CMAKE_CXX_STANDARD 17)
include_directories(/usr/include/readline)



add_executable(myshell meshell.cpp meshell.h)

target_link_libraries(myshell readline)
add_executable(myls myls.cpp myls.h utility.cpp utility.h)
add_executable(mycp mycp.cpp mycp.h utility.cpp utility.h)
add_executable(mymkdir mymkdir.cpp mymkdir.h utility.cpp utility.h)
add_executable(myrm myrm.cpp myrm.h utility.cpp utility.h)
add_executable(mymv mymv.cpp mymv.h utility.cpp utility.h)
add_executable(mygrep mygrep.cpp mygrep.h)
add_executable(mycat mycat.cpp)