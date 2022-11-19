cmake_minimum_required(VERSION 3.22.0)
project(app C)

set(CMAKE_C_STANDARD 99)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# add all executables from the Code directory
file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(app ${SOURCES})
target_link_libraries(app ${SDL2_LIBRARIES} -lSDL2_image)
