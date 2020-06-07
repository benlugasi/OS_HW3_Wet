cmake_minimum_required(VERSION 3.13)
project(hw3_wet)

set(CMAKE_CXX_FLAGS "--std=c++11 -pthread -Wall -pedantic -Werror -fPIC -pthread -std=c++11 -g -fsanitize=thread" )

add_executable(hw3_wet
        Barrier.cpp
        #TestBarrier.cpp
        tests/HeavyThreadsTest.cpp
        )