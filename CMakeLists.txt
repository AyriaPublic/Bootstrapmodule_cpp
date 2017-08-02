cmake_minimum_required(VERSION 3.1.0)
project(Bootstrapmodule_cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(LIBRARY_OUTPUT_PATH "Bin")

file(GLOB_RECURSE SOURCES "Source/*.cpp")

add_library(Nativebootstrap32 SHARED ${SOURCES})
add_library(Nativebootstrap64 SHARED ${SOURCES})

set_target_properties(Nativebootstrap32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
set_target_properties(Nativebootstrap64 PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
