cmake_minimum_required(VERSION 3.23)
project(raycasting)

set(CMAKE_C_STANDARD 99)
find_package(SDL2 REQUIRED)

add_executable(raycasting main.c)
target_link_libraries(raycasting ${SDL2_LIBRARIES})
