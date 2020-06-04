cmake_minimum_required(VERSION 3.13)
project(hw3_wet)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11 -pthread -Wall -pedantic -Werror")


add_executable(hw3_wet
        Barrier.cpp
        #TestBarrier.cpp
        testListHooks.cpp
        )