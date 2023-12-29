#include"GLFWManager.h"


GLFWManager::GLFWManager(unsigned int width, unsigned int height, const std::string& title) {
    Init(width, height, title.c_str());
}

GLFWManager::~GLFWManager() {
    Shutdown();
}

void GLFWManager::Init(unsigned int width, unsigned int height, const std::string& title) {
    glfwInit();
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }
    // Set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Uncomment this for MacOS compatibility
#endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); // For error handling

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);

}

void GLFWManager::Shutdown() 
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool GLFWManager::WindowShouldClose() 
{
    return glfwWindowShouldClose(window);
}

void GLFWManager::SwapBuffersAndPollEvents() 
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}

GLFWwindow* GLFWManager::GetWindow() const 
{
    return window;
}

void GLFWManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height) 
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void GLFWManager::ProcessInput() {
    // You can handle continuous key presses here
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void GLFWManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Handle single key events here
}
