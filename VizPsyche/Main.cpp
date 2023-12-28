#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include"Shader.h"
#include"VertexArray.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"
#include"Renderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void GLAPIENTRY errorHandleGL(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);

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
    Shader shader("default.shader");

    shader.Bind();
    // Unbind all to prevent accidentally modifying them
    vertexArray.Unbind();
    //shader.Unbind();
    //vertexBuffer.Unbind();
    //indexBuffer.Unbind();


    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Variables to be changed in the ImGUI window
    float clearColor[4] = { 0.05f, 0.02f, 0.01f, 1.0f };
    float color[4] = { 0.2f, 0.3f, 0.8f, 1.0f };

    //// Exporting variables to shaders
    //glUseProgram(shader.ID);
    ////glUniform4f(glGetUniformLocation(shader.ID, "color"), color[0], color[1], color[2], color[3]);
    shader.SetColor("color", color);
    shader.Unbind();


    // Error handling for OpenGL 4.3 and above https://docs.gl/gl4/glDebugMessageCallback
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(errorHandleGL, NULL);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // Tell OpenGL a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shader.Bind();
        // Bind the VertexArray so OpenGL knows to use it
        vertexArray.Bind();
        indexBuffer.Bind();
        // Draw primitives, number of indices, datatype of indices, index of indices

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glBindVertexArray(0); // no need to unbind it every time 


        // ImGUI window creation
        ImGui::Begin("ImGui Window");
        // Text that appears in the window
        ImGui::Text("Initialise ImGui Window");
        ImGui::ColorEdit4("Color", color);
        ImGui::ColorEdit4("Clear Color", clearColor);
        // Ends the window
        ImGui::End();

        //// Export variables to shader
        //glUseProgram(shader.ID);
        ////glUniform4f(glGetUniformLocation(shader.ID, "color"), color[0], color[1], color[2], color[3]);
        shader.SetColor("color", color);

        // Renders the ImGUI elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Deletes all ImGUI instances
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // Delete all the objects we've created
    vertexArray.Delete();

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


// Error handling for OpenGL 4.3 and above https://docs.gl/gl4/glDebugMessageCallback
void GLAPIENTRY errorHandleGL(GLenum source,
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