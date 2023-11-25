cmake_minimum_required(VERSION 3.15)
project(image_editor)
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# TODO: Собирать через промежутачный STATIC LIRARY target, чтобы избежать двойной компиляции

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -iquote ${PROJECT_SOURCE_DIR}/include")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -Ofast -march=native -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -march=native -fconcepts-diagnostics-depth=10")

find_package(SFML REQUIRED COMPONENTS graphics window system)
find_package(Qt6 REQUIRED COMPONENTS Widgets) 

file(GLOB SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(image_editor ${SRCS})
set_property(TARGET image_editor PROPERTY CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export-dynamic -ldl")
target_link_libraries(image_editor PRIVATE Qt6::Widgets sfml-graphics sfml-system sfml-window)

list(REMOVE_ITEM SRCS "${PROJECT_SOURCE_DIR}/src/main.cpp")

add_library(const_fill_plugin SHARED
        ${SRCS}
        "${PROJECT_SOURCE_DIR}/src/plugins/const_fill_plugin.cpp"
)

# target_link_libraries(const_fill_plugin PRIVATE Qt6::Widgets sfml-graphics sfml-system sfml-window)
target_include_directories(const_fill_plugin SYSTEM PRIVATE ${Qt6Widgets_INCLUDE_DIRS})
set_property(TARGET const_fill_plugin PROPERTY POSITION_INDEPENDENT_CODE ON)

add_custom_command(TARGET const_fill_plugin POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:const_fill_plugin>"
     "${PROJECT_SOURCE_DIR}/compiled_plugins/$<TARGET_FILE_NAME:const_fill_plugin>" 
  COMMENT "Copying to output directory")