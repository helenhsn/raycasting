cmake_minimum_required(VERSION 3.23)
project(raycasting)

set(CMAKE_C_STANDARD 99)
find_package(SDL2 REQUIRED)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})
file(GLOB SRC "src/*c")

add_executable(raycasting ${SRC} src/render.c include/render.h src/commands.c include/commands.h src/bezier.c include/bezier.h)
target_link_libraries(raycasting -lSDL2 -lSDL2_ttf)
target_link_libraries(raycasting m)
