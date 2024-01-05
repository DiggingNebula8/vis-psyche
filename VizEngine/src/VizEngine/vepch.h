#pragma once

// Standard Library Includes
#include <iostream>      // For input/output streams
#include <memory>        // For smart pointers like std::unique_ptr, std::shared_ptr
#include <utility>       // For utility functions like std::move, std::pair
#include <algorithm>     // For algorithms like std::sort, std::find
#include <functional>    // For std::function, std::bind
#include <sstream>       // For std::stringstream
#include <fstream>       // For file I/O via std::ifstream, std::ofstream
#include <string>        // For std::string
#include <vector>        // For std::vector
#include <unordered_map> // For std::unordered_map
#include <unordered_set> // For std::unordered_set
#include <array>         // For std::array
#include <map>           // For std::map

// C Standard Library
#include <cstdlib>       // General purpose functions like malloc, free
#include <cmath>         // Mathematical functions
#include <cassert>       // For assert function
#include <cerrno>        // For error handling

// Multithreading
#include <thread>        // For std::thread
#include <mutex>         // For std::mutex, std::lock_guard
#include <condition_variable> // For std::condition_variable

// Graphics and Window Management
#include <GLFW/glfw3.h>  // For GLFW functionalities
#include <glad/glad.h>   // For GLAD OpenGL loader
#include <../vendor/glm/glm/glm.hpp>   // For GLM, a mathematics library for graphics

// Platform-Specific Includes
#ifdef VP_PLATFORM_WINDOWS
#include <Windows.h> // For Windows-specific functionalities
#endif

// Logging and UI
#include <spdlog/spdlog.h> 
#include <imgui.h>         