cmake_minimum_required(VERSION 3.15)
project(image_editor)
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -iquote ${PROJECT_SOURCE_DIR}/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -Ofast -march=native -flto")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O3 -march=native -fconcepts-diagnostics-depth=10")

find_package(SFML REQUIRED COMPONENTS graphics window system)
find_package(Qt6 REQUIRED COMPONENTS Widgets) 

file(GLOB CORE_SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
)

list(REMOVE_ITEM CORE_SRCS "${PROJECT_SOURCE_DIR}/src/main.cpp")

##################
## Core library ##
##################

add_library(core_lib STATIC ${CORE_SRCS})
set_property(TARGET core_lib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(core_lib SYSTEM PRIVATE ${Qt6Widgets_INCLUDE_DIRS})

############################
## MAIN (HOST) executable ##
############################

add_executable(image_editor "${PROJECT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(image_editor PRIVATE core_lib Qt6::Widgets sfml-graphics sfml-system sfml-window)

##############################
## Const fill filter plugin ##
##############################

add_library(sphere_filter SHARED
        "${PROJECT_SOURCE_DIR}/src/plugins/sphere_filter.cpp"
)

target_link_libraries(sphere_filter PRIVATE core_lib)
set_property(TARGET sphere_filter PROPERTY POSITION_INDEPENDENT_CODE ON)

add_custom_command(TARGET sphere_filter POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:sphere_filter>"
     "${PROJECT_SOURCE_DIR}/compiled_plugins/$<TARGET_FILE_NAME:sphere_filter>" 
  COMMENT "Copying to output directory")

######################
## Fill tool plugin ##
######################

add_library(fill_tool_plugin SHARED
        "${PROJECT_SOURCE_DIR}/src/plugins/fill_tool.cpp"
        "${PROJECT_SOURCE_DIR}/assets/fill.png.cpp"
)

set_property(TARGET fill_tool_plugin PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(fill_tool_plugin PRIVATE core_lib)

add_custom_command(TARGET fill_tool_plugin POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:fill_tool_plugin>"
     "${PROJECT_SOURCE_DIR}/compiled_plugins/$<TARGET_FILE_NAME:fill_tool_plugin>" 
  COMMENT "Copying to output directory")

  ######################
## Fill tool plugin ##
######################

add_library(trip_filter_plugin SHARED
"${PROJECT_SOURCE_DIR}/src/plugins/trip_filter.cpp"
)

set_property(TARGET trip_filter_plugin PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(trip_filter_plugin PRIVATE core_lib)

add_custom_command(TARGET trip_filter_plugin POST_BUILD 
COMMAND "${CMAKE_COMMAND}" -E copy 
"$<TARGET_FILE:trip_filter_plugin>"
"${PROJECT_SOURCE_DIR}/compiled_plugins/$<TARGET_FILE_NAME:trip_filter_plugin>" 
COMMENT "Copying to output directory")
