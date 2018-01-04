cmake_minimum_required(VERSION 3.1.0)
project(Bootstrapmodule_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(LIBRARY_OUTPUT_PATH "Bin")

file(GLOB_RECURSE SOURCES "Source/*.cpp")

add_library(Nativebootstrap32 SHARED ${SOURCES})
add_library(Nativebootstrap64 SHARED ${SOURCES})

set_target_properties(Nativebootstrap32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
set_target_properties(Nativebootstrap64 PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
set_target_properties(Nativebootstrap32 Nativebootstrap64 PROPERTIES PREFIX "")

target_link_libraries(Nativebootstrap32 dl pthread)
target_link_libraries(Nativebootstrap64 dl pthread)
