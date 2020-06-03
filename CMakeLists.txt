cmake_minimum_required(VERSION 3.13)
project(barrier)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -pedantic -Werror –fPIC")

#add_custom_target(OS_Wet1 COMMAND make
#        CLION_EXE_DIR=${PROJECT_BINARY_DIR})


add_executable(barrier
        Barrier.cpp)