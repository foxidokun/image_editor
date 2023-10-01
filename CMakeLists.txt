cmake_minimum_required(VERSION 3.27)
project(image_editor)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)
find_package(SFML REQUIRED COMPONENTS graphics window system)
# find_package(OpenMP)

file(GLOB SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(vector ${SRCS})
target_link_libraries(vector PRIVATE sfml-graphics sfml-system sfml-window)
# target_link_libraries(vector PUBLIC OpenMP::OpenMP_CXX)