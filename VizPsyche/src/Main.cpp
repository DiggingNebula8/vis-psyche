#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include"Renderer.h"
#include"UIManager.h"
#include"ErrorHandling.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float vertices[] = {
     -0.5f,  -0.5f,  // top right
     0.5f, -0.5f,  // bottom right
    0.5f, 0.5f,  // bottom left
    -0.5f,  0.5f  // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 2,   // first triangle
    2, 3, 0    // second triangle
};


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); // for error handling



#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Viz Psyche", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    // Generates Vertex Array Object and binds it
    VertexArray vertexArray;
    // Generates Vertex Buffer Object and links it to vertices
    VertexBuffer vertexBuffer(vertices, 4 * 2 * sizeof(float));
    
    VertexBufferLayout layout;
    layout.Push<float>(2);
    // Links VertexBuffer to VertexArray
    vertexArray.LinkVertexBuffer(vertexBuffer, layout);
    // Generates Element Buffer Object and links it to indices
    IndexBuffer indexBuffer(indices, 6);
    // Generates Shader object
    Shader shader("src/resources/shaders/default.shader");

    shader.Bind();
    // Initialize UI
    UIManager uiManager(window);

    // Variables to be changed in the ImGUI window
    float clearColor[4] = { 0.05f, 0.02f, 0.01f, 1.0f };
    float color[4] = { 0.2f, 0.3f, 0.8f, 1.0f };
    shader.SetColor("color", color);
    // Unbind all to prevent accidentally modifying them
    shader.Unbind();
    vertexArray.Unbind();
    vertexBuffer.Unbind();
    indexBuffer.Unbind();


    Renderer renderer;


    // Error handling for OpenGL 4.3
    ErrorHandling::HandleErrors();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        renderer.Clear(clearColor);

        // Tell ImGUI that a new OpenGL frame is about to begin
        uiManager.BeginFrame();
        //ImGui_ImplOpenGL3_NewFrame();
        //ImGui_ImplGlfw_NewFrame();
        //ImGui::NewFrame();

        // Binding
        shader.Bind();
        vertexArray.Bind();
        indexBuffer.Bind();

        //Drawcall
        renderer.Draw(vertexArray, indexBuffer, shader);


        // ImGUI window creation
        uiManager.StartWindow("Simple UI Window");
        // Text that appears in the window
        ImGui::Text("Initialise ImGui Window");
        ImGui::ColorEdit4("Color", color);
        ImGui::ColorEdit4("Clear Color", clearColor);
        // Ends the window
        uiManager.EndWindow();

        //// Export variables to shader
        shader.SetColor("color", color);

        // Renders the ImGUI elements
        uiManager.Render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}