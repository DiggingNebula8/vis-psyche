#include"Application.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include"GUI/UIManager.h"

#include"OpenGL/GLFWManager.h"
#include"OpenGL/Renderer.h"
#include"OpenGL/Texture.h"
#include"OpenGL/ErrorHandling.h"
#include"glm.hpp"
#include"gtc//matrix_transform.hpp"
#include"gtc/type_ptr.hpp"

namespace VizEngine
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, /*Position*/  1.0f, 1.0f, 1.0f, 1.0f, /*Vertex Colors*/  0.0f, 0.0f, /*Texture Coords*/
         0.5f, -0.5f, 0.0f, /*Position*/  1.0f, 1.0f, 1.0f, 1.0f, /*Vertex Colors*/  1.0f, 0.0f, /*Texture Coords*/
         0.5f,  0.5f, 0.0f, /*Position*/  1.0f, 1.0f, 1.0f, 1.0f, /*Vertex Colors*/  1.0f, 1.0f, /*Texture Coords*/
        -0.5f,  0.5f, 0.0f, /*Position*/  1.0f, 1.0f, 1.0f, 1.0f, /*Vertex Colors*/  0.0f, 1.0f  /*Texture Coords*/
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
        2, 3, 0    // second triangle
    };

	int Application::Run()
	{
        GLFWManager glfw(SCR_WIDTH, SCR_HEIGHT, "Viz Psyche");

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Generates Vertex Array Object and binds it
        VertexArray vertexArray;
        // Generates Vertex Buffer Object and links it to vertices
        VertexBuffer vertexBuffer(vertices, 4 * 9 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(4);
        layout.Push<float>(2);
        // Links VertexBuffer to VertexArray
        vertexArray.LinkVertexBuffer(vertexBuffer, layout);
        // Generates Element Buffer Object and links it to indices
        IndexBuffer indexBuffer(indices, 6);

        glm::mat4 projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

        // Generates Shader object
        Shader shader("src/resources/shaders/default.shader");

        shader.Bind();
        Texture texture("src/resources/textures/hellotexture.png");
        texture.Bind();

        // Initialize UI
        UIManager uiManager(glfw.GetWindow());

        // Variables to be changed in the ImGUI window
        float clearColor[4] = { 0.05f, 0.02f, 0.01f, 1.0f };
        glm::vec4 color = { 0.2f, 0.3f, 0.8f, 1.0f };
        float scale = 1.0f;
        shader.SetColor("u_Color", color);
        shader.SetFloat("u_Scale", scale);
        shader.SetMatrix4f("u_MVP", projectionMatrix);
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
        while (!glfw.WindowShouldClose())
        {
            // Inputs
            glfw.ProcessInput();
            uiManager.BeginFrame();

            // Rendering
            renderer.Clear(clearColor);


            // Binding
            shader.Bind();
            vertexArray.Bind();
            indexBuffer.Bind();

            //Drawcall
            renderer.Draw(vertexArray, indexBuffer, shader);


            // ImGUI window creation
            uiManager.StartWindow("Simple UI Window");
            // Text that appears in the window
            ImGui::Text("Background Parameters");
            ImGui::Spacing();
            ImGui::ColorEdit4("Clear Color", clearColor);
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Text("Image Parameters");
            ImGui::Spacing();
            ImGui::SliderFloat("Scale", &scale, 0.0f, 2.0f);
            ImGui::ColorEdit4("Colour", glm::value_ptr(color));
            // Ends the window
            uiManager.EndWindow();

            //// Export variables to shader
            shader.SetColor("u_Color", color);
            shader.SetFloat("u_Scale", scale);

            // Renders the ImGUI elements
            uiManager.Render();

            glfw.SwapBuffersAndPollEvents();
        }
        return 0;
	}
}
