#include"ErrorHandling.h"

// Error handling for OpenGL 4.3 and above https://docs.gl/gl4/glDebugMessageCallback
void GLAPIENTRY ErrorHandling::ErrorHandler(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    std::cout << "OpenGL Error:\n"
        << "Source: 0x" << std::hex << source << "\n"
        << "Type: 0x" << std::hex << type << "\n"
        << "Id: 0x" << std::hex << id << "\n"
        << "Severity: 0x" << std::hex << severity << "\n";
    std::cout << message << "\n";

    exit(-1); // shut down the program gracefully (it does cleanup stuff too)
    // without exit(), OpenGL will constantly print the error message... make sure to kill your program.
}

void ErrorHandling::HandleErrors()
{
    // Error handling for OpenGL 4.3 and above https://docs.gl/gl4/glDebugMessageCallback
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(ErrorHandling::ErrorHandler, NULL);
}