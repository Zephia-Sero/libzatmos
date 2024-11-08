﻿cmake_minimum_required (VERSION 3.11)
project (zatmos VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE INC_FILES "src/*.hpp")
file(GLOB_RECURSE DEMO_SRC_FILES "demo/*.cpp")

# Add source files to the library
add_library(zatmos SHARED ${SRC_FILES})
add_executable(libzatmos-demo ${DEMO_SRC_FILES})

# Allow CMake to append version # to filename
set_target_properties(zatmos PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libzatmos-demo PROPERTIES VERSION ${PROJECT_VERSION})

# Semantic versioning for previous
set_target_properties(zatmos PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(libzatmos-demo PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# Set external include files
set_target_properties(zatmos PROPERTIES PUBLIC_HEADER "src/*.hpp")
set_target_properties(libzatmos-demo PROPERTIES PUBLIC_HEADER "src/*.hpp")

# Set the include directory for the library itself
target_include_directories(zatmos PRIVATE "src")
target_include_directories(libzatmos-demo PRIVATE "src" "demo")

# Libraries for demo
add_subdirectory(${PROJECT_SOURCE_DIR}/raylib/)
target_link_libraries(libzatmos-demo PRIVATE zatmos raylib)
include_directories(${PROJECT_SOURCE_DIR}/raylib/src/)

# Install headers and SOs to system directories
include(GNUInstallDirs)
install(TARGETS zatmos
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
