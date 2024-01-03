project(VizEngine)

# Copy GLFW headers from vendor/glfw/include to include/GLFW
file(COPY ${CMAKE_SOURCE_DIR}/VizEngine/vendor/glfw/include/ 
    DESTINATION ${CMAKE_SOURCE_DIR}/VizEngine/include/)

# Copy the GLFW lib file
file(COPY ${CMAKE_SOURCE_DIR}/VizEngine/vendor/glfw/build/src/Debug/glfw3.lib
    DESTINATION ${CMAKE_SOURCE_DIR}/VizEngine/lib/)

# Copy the GLFW pdb file
file(COPY ${CMAKE_SOURCE_DIR}/VizEngine/vendor/glfw/build/src/Debug/glfw3.pdb
    DESTINATION ${CMAKE_SOURCE_DIR}/VizEngine/lib/)



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
    vendor/spdlog/include
    vendor/imgui
    vendor/imgui/backends
    vendor/stb_image
)

# Add definitions and link libraries as needed
target_compile_definitions(VizEngine PRIVATE
    VP_PLATFORM_WINDOWS
    VP_BUILD_DLL
)
# Specify the library directories (if you have pre-compiled libs in lib)
target_link_directories(VizEngine PRIVATE lib)
# Link external libraries (adjust according to your needs)
target_link_libraries(VizEngine PRIVATE glfw3 opengl32)