# Sets the minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.10)

# Set the project name and languages
project(volleybot_physics LANGUAGES C CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a list of all your source files
set(SOURCE_FILES
    # C Core Files
    src/physics_core/vec3.c
    src/physics_core/mat4.c
    src/physics_core/vec2.c
    src/physics_core/kinematics.c
    src/physics_core/collision.c

    # C++ Implementation Files
    src/volleybot_physics/primitive.cpp
    src/volleybot_physics/scene.cpp
    src/volleybot_physics/camera.cpp
    src/volleybot_physics/light.cpp
)

# Add a library target. We build a SHARED library so it can be loaded by Python.
add_library(volleybot_physics SHARED ${SOURCE_FILES})

# Specify the include directories for the compiler
target_include_directories(volleybot_physics PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --- Optional: For later when you add tinyobjloader ---
# 1. Create a folder 'external' and place tiny_obj_loader.h inside.
# 2. Uncomment the line below.
# target_include_directories(volleybot_physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external)


# A message to show when CMake configures the project
message(STATUS "Configuration complete for volleybot_physics library.")
message(STATUS "To build, create a build directory, run 'cmake ..' from it, then 'cmake --build .'")
