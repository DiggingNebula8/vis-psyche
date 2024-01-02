project(SandboxApp)

# Add source files for the Sandbox application
set(SANDBOX_SOURCES "src/SandboxApp.cpp")

# Create an executable
add_executable(Sandbox ${SANDBOX_SOURCES})

# Link with the VizEngine engine
target_link_libraries(Sandbox PRIVATE VizEngine)

# Include directories from the engine
target_include_directories(Sandbox PRIVATE
    ../../VizEngine/include
    ../../VizEngine/vendor/spdlog/include
    ../../VizEngine/vendor/imgui
)

target_compile_definitions(Sandbox PRIVATE
    VP_PLATFORM_WINDOWS
)

# Post-build command to copy the src directory to the Sandbox solution directory
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/VizEngine/src" "${CMAKE_CURRENT_BINARY_DIR}/src"
)

# Post-build command to copy shader files
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Sandbox>/src/resources/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/VizEngine/src/resources/shaders" "$<TARGET_FILE_DIR:Sandbox>/src/resources/shaders"
)

# Post-build command to copy texture files
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Sandbox>/src/resources/textures/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/VizEngine/src/resources/textures" "$<TARGET_FILE_DIR:Sandbox>/src/resources/textures"
)

# Post-build command to copy VizEngine.dll to Sandbox executable directory
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:VizEngine>"
    "$<TARGET_FILE_DIR:Sandbox>"
)
