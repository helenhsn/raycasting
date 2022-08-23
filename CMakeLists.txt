cmake_minimum_required(VERSION 3.23)
project(raycasting)

set(CMAKE_C_STANDARD 99)
find_package(SDL2 REQUIRED)

include_directories(include)
file(GLOB SRC "src/*c")

add_executable(raycasting ${SRC})
target_link_libraries(raycasting ${SDL2_LIBRARIES})
