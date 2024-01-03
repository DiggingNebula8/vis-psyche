project(VizEngine)

# Include your engine source files and the vendor source files
file(GLOB_RECURSE ENGINE_SOURCES 
    "src/*.cpp"
    "src/*.c"
    "vendor/imgui/imgui.cpp"
    "vendor/imgui/imgui_demo.cpp"
    "vendor/imgui/imgui_draw.cpp"
    "vendor/imgui/backends/imgui_impl_glfw.cpp"
    "vendor/imgui/backends/imgui_impl_opengl3.cpp"
    "vendor/imgui/imgui_tables.cpp"
    "vendor/imgui/imgui_widgets.cpp"
    "vendor/stb_image/*.cpp"
)

# Create a library for the engine
add_library(VizEngine SHARED ${ENGINE_SOURCES})

# Add include directories
target_include_directories(VizEngine PUBLIC
    include
    src/
    vendor/glfw/include/
    vendor/spdlog/include
    vendor/imgui
    vendor/imgui/backends
    vendor/stb_image
    vendor/glm/glm
)

# Add definitions and link libraries as needed
target_compile_definitions(VizEngine PRIVATE
    VP_PLATFORM_WINDOWS
    VP_BUILD_DLL
)

# Link external libraries (adjust according to your needs)
target_link_libraries(VizEngine PRIVATE glfw opengl32)