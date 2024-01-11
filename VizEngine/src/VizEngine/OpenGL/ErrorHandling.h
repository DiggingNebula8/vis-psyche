#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define ASSERT(x) if (!(x)) __debugbreak();

class ErrorHandling
{
public:
    static void GLAPIENTRY ErrorHandler(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam);

    static void HandleErrors();

};
