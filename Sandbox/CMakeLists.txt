project(Sandbox)

# =============================================================================
# Source Files
# =============================================================================
set(SANDBOX_SOURCES
    src/SandboxApp.cpp
)

# =============================================================================
# Executable Target
# =============================================================================
add_executable(Sandbox ${SANDBOX_SOURCES})

# =============================================================================
# Link Libraries
# =============================================================================
# VizEngine propagates ImGui via PUBLIC linkage, so Sandbox gets it automatically
target_link_libraries(Sandbox PRIVATE VizEngine)

# =============================================================================
# Compile Definitions
# =============================================================================

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Sandbox PRIVATE
    $<$<PLATFORM_ID:Windows>:VP_PLATFORM_WINDOWS>
)

# =============================================================================
# Compiler Warnings
# =============================================================================
if(MSVC)
    target_compile_options(Sandbox PRIVATE /W4 /utf-8)
else()
    target_compile_options(Sandbox PRIVATE -Wall -Wextra -Wpedantic)
endif()

# =============================================================================
# Visual Studio Debugger Configuration
# =============================================================================
# Set working directory to where the executable and resources are located
set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Sandbox>"
)

# =============================================================================
# Post-Build: Copy Resources
# =============================================================================
# Copy shaders and textures to output directory
add_custom_command(TARGET Sandbox POST_BUILD
    COMMENT "Copying resources to output directory..."
    # Copy shaders and textures
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Sandbox>/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_SOURCE_DIR}/VizEngine/src/resources" 
        "$<TARGET_FILE_DIR:Sandbox>/resources"
    # Copy glTF sample assets (only Duck for now to save space)
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Sandbox>/assets/gltf-samples/Models/Duck/glTF-Binary"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/VizEngine/assets/gltf-samples/Models/Duck/glTF-Binary/Duck.glb"
        "$<TARGET_FILE_DIR:Sandbox>/assets/gltf-samples/Models/Duck/glTF-Binary/"
)
