cmake_minimum_required(VERSION 3.1)
project(SysOpy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(SysOpy ${SOURCE_FILES} lab1/list_funs.h lab1/list_funs.c lab1/main.h lab1/main.c lab1/list_structs.h lab1/list_structs.c lab1/Makefile.cpp lab1/Makefile.h lab1/src/utils.h lab1/src/utils.c)