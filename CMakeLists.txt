cmake_minimum_required(VERSION 3.1)
project(Bootstrapmodule_cpp)

# Use the latest standard at this time.
set(CMAKE_CXX_STANDARD 17)

# Export to the a gitignored directory.
set(EXECUTABLE_OUTPUT_PATH ${Bootstrapmodule_cpp_SOURCE_DIR}/Bin)
set(LIBRARY_OUTPUT_PATH ${Bootstrapmodule_cpp_SOURCE_DIR}/Bin)

# Set warningflags per compiler.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

# Separate release and debugbuilds output and dependencies.
mark_as_advanced(CMAKE_IMPORT_LIBRARY_SUFFIX CMAKE_DEBUG_POSTFIX)
set(CMAKE_DEBUG_POSTFIX "d")

# Just pull all the files from /Source
file(GLOB_RECURSE SOURCES "Source/*.cpp")

# Build both versions of the bootstrapmodule.
add_library(Nativebootstrap32 SHARED ${SOURCES})
add_library(Nativebootstrap64 SHARED ${SOURCES})

# Remove the 'lib' prefix for the files.
set_target_properties(Nativebootstrap32 Nativebootstrap64 PROPERTIES PREFIX "")

# Differentiate between x64 and x86 builds.
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set_target_properties(Nativebootstrap32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    set_target_properties(Nativebootstrap64 PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
endif()

# Library dependencies for the platforms.
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    target_link_libraries(Nativebootstrap32 dl pthread)
    target_link_libraries(Nativebootstrap64 dl pthread)
else()

endif()

# Use static VC runtimes when releasing on Windows.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()
