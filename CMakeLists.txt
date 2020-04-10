cmake_minimum_required(VERSION 3.15)
project(VoxEngine C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-O3")

add_executable(VoxEngine main.c common.h glUtils.h map.h noise.h octree.h)

target_link_libraries(VoxEngine -lGL -lGLEW -lglfw -lm -lpthread)
