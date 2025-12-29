cmake_minimum_required(VERSION 3.16)
project(VizPsyche VERSION 1.0.0 LANGUAGES C CXX)

# =============================================================================
# C++ Standard
# =============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# Output Directories
# =============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# =============================================================================
# Platform Configuration
# =============================================================================
if(WIN32)
    add_definitions(-DNOMINMAX)        # Prevent Windows.h min/max macros
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# =============================================================================
# Git Submodules
# =============================================================================
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Update submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Updating git submodules...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT
        )
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(WARNING "git submodule update failed: ${GIT_SUBMOD_RESULT}")
        endif()
    endif()
endif()

# =============================================================================
# GLFW Configuration (before adding subdirectory)
# =============================================================================
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# =============================================================================
# Subdirectories
# =============================================================================
add_subdirectory(VizEngine/vendor/glfw)
add_subdirectory(VizEngine)
add_subdirectory(Sandbox)

# =============================================================================
# IDE Configuration
# =============================================================================
# Set Sandbox as startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)

# Organize targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(glfw PROPERTIES FOLDER "Dependencies")
if(TARGET update_mappings)
    set_target_properties(update_mappings PROPERTIES FOLDER "Dependencies")
endif()

# =============================================================================
# Configuration Summary
# =============================================================================
message(STATUS "")
message(STATUS "╔══════════════════════════════════════════╗")
message(STATUS "║         VizPsyche Configuration          ║")
message(STATUS "╠══════════════════════════════════════════╣")
message(STATUS "║  Version:      ${PROJECT_VERSION}                       ║")
message(STATUS "║  C++ Standard: ${CMAKE_CXX_STANDARD}                         ║")
message(STATUS "║  Build Type:   ${CMAKE_BUILD_TYPE}                          ║")
message(STATUS "║  Generator:    ${CMAKE_GENERATOR}")
message(STATUS "╚══════════════════════════════════════════╝")
message(STATUS "")
