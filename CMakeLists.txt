cmake_minimum_required(VERSION 3.16)

project(MarchingCubes
        LANGUAGES CXX C)

option(BUILD_TEST "build test for marching cube and volume render " OFF)
option(BUILD_EXAMPLE "build example exe for marching cube and volume render" ON)

add_subdirectory(src)

if(BUILD_TEST)
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
