cmake_minimum_required(VERSION 3.27)
project(image_editor)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -iquote ${PROJECT_SOURCE_DIR}/include -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -iquote ${PROJECT_SOURCE_DIR}/include")

include_directories(${CMAKE_SOURCE_DIR}/lib/nativefiledialog/src/include)
find_package(SFML REQUIRED COMPONENTS graphics window system)
find_package(Qt6 REQUIRED COMPONENTS Widgets) 
# find_package(OpenMP)

file(GLOB SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(image_editor ${SRCS})
target_link_libraries(image_editor PRIVATE Qt6::Widgets sfml-graphics sfml-system sfml-window glib-2.0 gtk-3 ${CMAKE_SOURCE_DIR}/lib/nativefiledialog/build/lib/Release/x64/libnfd.a)
# target_link_libraries(vector PUBLIC OpenMP::OpenMP_CXX)