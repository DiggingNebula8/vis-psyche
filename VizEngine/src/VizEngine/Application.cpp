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

#include"Events/AppEvent.h"
#include"Log.h"


namespace VizEngine
{
#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)
	Application::Application()
	{
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));
	}

    void Application::OnEvent(Event& e)
    {

    }

	Application::~Application()
	{

	}

    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 800;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // Base of the Pyramid
        // Position             Color                   Texture Coords
        -0.5f, 0.0f,  0.5f, 1.0f, /**/ 1.0f, 1.0f, 1.0f, 1.0f, /**/ 0.0f, 0.0f, // Bottom Left
        -0.5f, 0.0f, -0.5f, 1.0f, /**/ 1.0f, 1.0f, 1.0f, 1.0f, /**/ 0.0f, 2.0f, // Top Left
         0.5f, 0.0f, -0.5f, 1.0f, /**/ 1.0f, 1.0f, 1.0f, 1.0f, /**/ 2.0f, 2.0f, // Top Right
         0.5f, 0.0f,  0.5f, 1.0f, /**/ 1.0f, 1.0f, 1.0f, 1.0f, /**/ 2.0f, 0.0f, // Bottom Right

         // Tip of the Pyramid
         0.0f, 0.8f,  0.0f, 1.0f, /**/ 1.0f, 1.0f, 1.0f, 1.0f, /**/ 1.0f, 1.0f  // Tip
    };


    unsigned int indices[] = {
        0, 1, 2,   // base
        0, 2, 3,
        0, 1, 4,   // sides
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };


	int Application::Run()
	{
        while (m_Running)
        {
            m_Window->OnUpdate();
        }
        //WindowResizeEvent e(SCR_WIDTH, SCR_HEIGHT);
        //VE_CORE_DEBUG(e);
        //GLFWManager glfw(SCR_WIDTH, SCR_HEIGHT, "Viz Psyche");

        //// glad: load all OpenGL function pointers
        //// ---------------------------------------
        //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        //{
        //    std::cout << "Failed to initialize GLAD" << std::endl;
        //    return -1;
        //}

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glEnable(GL_DEPTH_TEST);

        //// Generates Vertex Array Object and binds it
        //VertexArray vertexArray;
        //// Generates Vertex Buffer Object and links it to vertices
        //VertexBuffer vertexBuffer(vertices, 5 * 10 * sizeof(float));

        //VertexBufferLayout layout;
        //layout.Push<float>(4);
        //layout.Push<float>(4);
        //layout.Push<float>(2);
        //// Links VertexBuffer to VertexArray
        //vertexArray.LinkVertexBuffer(vertexBuffer, layout);
        //// Generates Element Buffer Object and links it to indices
        //IndexBuffer indexBuffer(indices, 18);

        //// Generates Shader object
        //Shader shader("src/resources/shaders/default.shader");

        //shader.Bind();
        //glm::mat4 model = glm::mat4(1.0f);
        //glm::mat4 view = glm::mat4(1.0f);
        //glm::mat4 proj = glm::mat4(1.0f);
        //view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
        //proj = glm::perspective(glm::radians(45.0f), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
        //glm::mat4 u_MVP = proj * view * model;
        //shader.SetMatrix4fv("u_MVP", u_MVP);


        //Texture texture("src/resources/textures/uvchecker.png");
        //texture.Bind();

        //// Initialize UI
        //UIManager uiManager(glfw.GetWindow());

        //// Variables to be changed in the ImGUI window
        //float clearColor[4] = { 0.05f, 0.02f, 0.01f, 1.0f };
        //glm::vec4 color = { 0.2f, 0.3f, 0.8f, 1.0f };
        //float rotation = 0.0f;
        //float rotationSpeed = 0.1f;
        //double prevTime = glfwGetTime();
        //shader.SetColor("u_Color", color);
        //// Unbind all to prevent accidentally modifying them
        //shader.Unbind();
        //vertexArray.Unbind();
        //vertexBuffer.Unbind();
        //indexBuffer.Unbind();


        //Renderer renderer;


        //// Error handling for OpenGL 4.3
        //ErrorHandling::HandleErrors();

        //// render loop
        //// -----------
        //while (!glfw.WindowShouldClose())
        //{
        //    // Inputs
        //    glfw.ProcessInput();
        //    uiManager.BeginFrame();

        //    // Rendering
        //    renderer.Clear(clearColor);
        //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //    // Binding
        //    shader.Bind();

        //    double crntTime = glfwGetTime();
        //    if (crntTime - prevTime >= 1 / 60)
        //    {
        //        rotation += rotationSpeed;
        //        prevTime = crntTime;
        //    }
        //    glm::mat4 model = glm::mat4(1.0f);
        //    glm::mat4 view = glm::mat4(1.0f);
        //    glm::mat4 proj = glm::mat4(1.0f);
        //    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        //    view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
        //    proj = glm::perspective(glm::radians(45.0f), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
        //    glm::mat4 u_MVP = proj * view * model;
        //    shader.SetMatrix4fv("u_MVP", u_MVP);
        //    vertexArray.Bind();
        //    indexBuffer.Bind();

        //    //Drawcall
        //    renderer.Draw(vertexArray, indexBuffer, shader);


        //    // ImGUI window creation
        //    uiManager.StartWindow("Simple UI Window");
        //    // Text that appears in the window
        //    ImGui::Text("Background Parameters");
        //    ImGui::Spacing();
        //    ImGui::ColorEdit4("Clear Color", clearColor);
        //    ImGui::Spacing();
        //    ImGui::Spacing();
        //    ImGui::Text("Parameters");
        //    ImGui::Spacing();
        //    ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 1.0f);
        //    ImGui::ColorEdit4("Colour", glm::value_ptr(color));
        //    // Ends the window
        //    uiManager.EndWindow();

        //    //// Export variables to shader
        //    shader.SetColor("u_Color", color);

        //    // Renders the ImGUI elements
        //    uiManager.Render();

        //    glfw.SwapBuffersAndPollEvents();
        //}
        return 0;
	}
}
