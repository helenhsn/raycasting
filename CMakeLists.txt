cmake_minimum_required(VERSION 3.22)
project(raycasting)

set(CMAKE_C_STANDARD 99)
find_package(SDL2 REQUIRED)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})
file(GLOB SRC "src/*c")

add_executable(raycasting ${SRC} src/render.c include/render.h src/commands.c include/commands.h src/bezier.c include/bezier.h src/intersection.c include/intersection.h)
target_link_libraries(raycasting -lSDL2 -lSDL2_ttf -lgsl m)
SET(GCC_COVERAGE_COMPILE_FLAGS "-fno-stack-protector")
SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
