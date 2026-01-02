project(VizEngine)

# =============================================================================
# Source Files (explicit list — more reliable than GLOB_RECURSE)
# =============================================================================
set(VIZENGINE_SOURCES
    # Core
    src/VizEngine/Application.cpp
    src/VizEngine/Log.cpp
    src/VizEngine/Core/Camera.cpp
    src/VizEngine/Core/Mesh.cpp
    src/VizEngine/Core/Scene.cpp
    src/VizEngine/Core/Model.cpp
    src/VizEngine/Core/TinyGLTF.cpp
    src/VizEngine/Core/Input.cpp
    
    # OpenGL
    src/VizEngine/OpenGL/glad.c
    src/VizEngine/OpenGL/ErrorHandling.cpp
    src/VizEngine/OpenGL/GLFWManager.cpp
    src/VizEngine/OpenGL/IndexBuffer.cpp
    src/VizEngine/OpenGL/Renderer.cpp
    src/VizEngine/OpenGL/Shader.cpp
    src/VizEngine/OpenGL/Texture.cpp
    src/VizEngine/OpenGL/VertexArray.cpp
    src/VizEngine/OpenGL/VertexBuffer.cpp
    
    # GUI
    src/VizEngine/GUI/UIManager.cpp
)

set(VIZENGINE_HEADERS
    # Public headers
    src/VizEngine.h
    src/VizEngine/Application.h
    src/VizEngine/Core.h
    src/VizEngine/EntryPoint.h
    src/VizEngine/Log.h
    
    # Core headers
    src/VizEngine/Core/Camera.h
    src/VizEngine/Core/Mesh.h
    src/VizEngine/Core/Transform.h
    src/VizEngine/Core/Scene.h
    src/VizEngine/Core/SceneObject.h
    src/VizEngine/Core/Light.h
    src/VizEngine/Core/Material.h
    src/VizEngine/Core/Model.h
    src/VizEngine/Core/Input.h
    
    # OpenGL headers
    src/VizEngine/OpenGL/Commons.h
    src/VizEngine/OpenGL/ErrorHandling.h
    src/VizEngine/OpenGL/GLFWManager.h
    src/VizEngine/OpenGL/IndexBuffer.h
    src/VizEngine/OpenGL/Renderer.h
    src/VizEngine/OpenGL/Shader.h
    src/VizEngine/OpenGL/Texture.h
    src/VizEngine/OpenGL/VertexArray.h
    src/VizEngine/OpenGL/VertexBuffer.h
    src/VizEngine/OpenGL/VertexBufferLayout.h
    
    # GUI headers
    src/VizEngine/GUI/UIManager.h
    
    # GLAD
    include/glad/glad.h
    include/KHR/khrplatform.h
)

# Vendor sources (ImGui + stb)
set(VENDOR_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
    vendor/stb_image/stb_image.cpp
)

# =============================================================================
# Library Target
# =============================================================================
add_library(VizEngine SHARED 
    ${VIZENGINE_SOURCES} 
    ${VIZENGINE_HEADERS}
    ${VENDOR_SOURCES}
)

# Group files in IDE
source_group("Source Files" FILES ${VIZENGINE_SOURCES})
source_group("Header Files" FILES ${VIZENGINE_HEADERS})
source_group("Vendor" FILES ${VENDOR_SOURCES})

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(VizEngine
    PUBLIC
        # Headers consumers need
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/glm>  # GLM used in public headers
    PRIVATE
        # Internal only — not exposed to consumers
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tinygltf
)

# =============================================================================
# Compile Definitions
# =============================================================================
# Platform detection - use WIN32 for reliable Windows detection
if(WIN32)
    set(PLATFORM_DEFINITIONS VP_PLATFORM_WINDOWS)
endif()

target_compile_definitions(VizEngine
    PUBLIC
        ${PLATFORM_DEFINITIONS}
    PRIVATE
        VP_BUILD_DLL
        # Silence MSVC warnings in vendor code
        _CRT_SECURE_NO_WARNINGS
)

# =============================================================================
# Compiler Warnings
# =============================================================================
if(MSVC)
    target_compile_options(VizEngine PRIVATE 
        /W4           # High warning level
        /utf-8        # Required for spdlog/fmt Unicode support
        /wd4251       # DLL-interface warning (expected with spdlog)
        /wd4275       # Non-DLL-interface base class
    )
else()
    target_compile_options(VizEngine PRIVATE 
        -Wall -Wextra -Wpedantic
    )
endif()

# =============================================================================
# Link Libraries
# =============================================================================
target_link_libraries(VizEngine 
    PRIVATE 
        glfw
        $<$<PLATFORM_ID:Windows>:opengl32>
        $<$<PLATFORM_ID:Linux>:GL>
        $<$<PLATFORM_ID:Darwin>:-framework OpenGL>
)

# =============================================================================
# Resources
# =============================================================================
# Define resource directories for consumers
set(VIZENGINE_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/resources" CACHE PATH "VizEngine resources directory")
