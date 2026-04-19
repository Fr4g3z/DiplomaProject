cmake_minimum_required(VERSION 3.16)

project(Hypercube4DViewer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Debug build if none is specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build (Debug or Release)" FORCE)
endif()

# ---- Dependencies ----
# Find system OpenGL (this gives you OpenGL::GL on modern CMake) [web:48]
find_package(OpenGL REQUIRED)

# Assume GLFW and GLAD are added as subdirectories in external/
# external/glfw  -> official GLFW repo (with its own CMakeLists)
# external/glad  -> your own small CMakeLists that builds a 'glad' library
add_subdirectory(external/glfw)
add_subdirectory(external/glad)

# ---- Sources ----
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.c
)

# WIN32 tells MSVC/Linker to build a GUI app (no console window)
add_executable(Hypercube4DViewer ${SRC_FILES})

# Include directory for your headers
target_include_directories(Hypercube4DViewer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against GLFW, GLAD, and OpenGL
target_link_libraries(Hypercube4DViewer
    PRIVATE
        glfw
        glad
        OpenGL::GL
)

# Extra warnings on MSVC
if (MSVC)
    target_compile_options(Hypercube4DViewer PRIVATE /W4 /permissive-)
    # Optional: silence some CRT warnings
    target_compile_definitions(Hypercube4DViewer PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# ---- Install rule ----
# This will install the .exe into the chosen prefix.
# We'll use it to populate ../ForUser
install(TARGETS Hypercube4DViewer
    RUNTIME DESTINATION .
)