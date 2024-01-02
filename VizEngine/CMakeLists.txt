project(VizPsyche)

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
add_library(VizPsyche SHARED ${ENGINE_SOURCES})

# Add include directories
target_include_directories(VizPsyche PUBLIC
    include
    src/
    vendor/spdlog/include
    vendor/imgui
    vendor/imgui/backends
    vendor/stb_image
)

# Add definitions and link libraries as needed
target_compile_definitions(VizPsyche PRIVATE
    VP_PLATFORM_WINDOWS
    VP_BUILD_DLL
)
# Specify the library directories (if you have pre-compiled libs in lib)
target_link_directories(VizPsyche PRIVATE lib)
# Link external libraries (adjust according to your needs)
target_link_libraries(VizPsyche PRIVATE glfw3 opengl32)