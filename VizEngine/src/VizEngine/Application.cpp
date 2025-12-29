#include "Application.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GUI/UIManager.h"
#include "Core/Camera.h"
#include "Core/Mesh.h"
#include "Core/Transform.h"

#include "OpenGL/GLFWManager.h"
#include "OpenGL/Renderer.h"
#include "OpenGL/Texture.h"
#include "OpenGL/ErrorHandling.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

namespace VizEngine
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	// Window settings
	static const unsigned int SCR_WIDTH = 800;
	static const unsigned int SCR_HEIGHT = 800;

	int Application::Run()
	{
		// =========================================================================
		// Initialization
		// =========================================================================
		GLFWManager window(SCR_WIDTH, SCR_HEIGHT, "Viz Psyche");

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		// =========================================================================
		// Create Scene Objects
		// =========================================================================
		
		// Camera
		Camera camera(45.0f, static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
		camera.SetPosition(glm::vec3(0.0f, 4.0f, -15.0f));

		// Mesh (using factory method)
		auto pyramidMesh = Mesh::CreatePyramid();

		// Transform for the pyramid
		Transform pyramidTransform;
		pyramidTransform.Position = glm::vec3(0.0f, 0.0f, 0.0f);
		pyramidTransform.Scale = glm::vec3(5.0f, 10.0f, 5.0f);

		// =========================================================================
		// Load Assets
		// =========================================================================
		Shader shader("src/resources/shaders/default.shader");
		Texture texture("src/resources/textures/uvchecker.png");
		texture.Bind();

		// =========================================================================
		// UI & Renderer
		// =========================================================================
		UIManager uiManager(window.GetWindow());
		Renderer renderer;

		// =========================================================================
		// Runtime Variables
		// =========================================================================
		float clearColor[4] = { 0.05f, 0.02f, 0.01f, 1.0f };
		glm::vec4 objectColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		float rotationSpeed = 0.5f;
		double prevTime = glfwGetTime();

		// Enable OpenGL debug output
		ErrorHandling::HandleErrors();

		// =========================================================================
		// Main Render Loop
		// =========================================================================
		while (!window.WindowShouldClose())
		{
			// --- Input ---
			window.ProcessInput();
			uiManager.BeginFrame();

			// --- Update ---
			double currentTime = glfwGetTime();
			float deltaTime = static_cast<float>(currentTime - prevTime);
			prevTime = currentTime;

			// Rotate the pyramid
			pyramidTransform.Rotation.y += rotationSpeed * deltaTime;

			// Calculate MVP matrix
			glm::mat4 model = pyramidTransform.GetModelMatrix();
			glm::mat4 mvp = camera.GetViewProjectionMatrix() * model;

			// --- Render ---
			renderer.Clear(clearColor);

			shader.Bind();
			shader.SetMatrix4fv("u_MVP", mvp);
			shader.SetColor("u_Color", objectColor);

			pyramidMesh->Bind();
			renderer.Draw(pyramidMesh->GetVertexArray(), pyramidMesh->GetIndexBuffer(), shader);

			// --- UI ---
			uiManager.StartWindow("Scene Controls");
			
			ImGui::Text("Background");
			ImGui::ColorEdit4("Clear Color", clearColor);
			ImGui::Separator();
			
			ImGui::Text("Object");
			ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 5.0f);
			ImGui::ColorEdit4("Object Color", glm::value_ptr(objectColor));
			ImGui::Separator();
			
			ImGui::Text("Transform");
			ImGui::DragFloat3("Position", glm::value_ptr(pyramidTransform.Position), 0.1f);
			glm::vec3 rotDegrees = pyramidTransform.GetRotationDegrees();
			if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotDegrees), 1.0f))
			{
				pyramidTransform.SetRotationDegrees(rotDegrees);
			}
			ImGui::DragFloat3("Scale", glm::value_ptr(pyramidTransform.Scale), 0.1f, 0.1f, 10.0f);
			ImGui::Separator();

			ImGui::Text("Camera");
			glm::vec3 camPos = camera.GetPosition();
			if (ImGui::DragFloat3("Camera Pos", glm::value_ptr(camPos), 0.1f))
			{
				camera.SetPosition(camPos);
			}

			uiManager.EndWindow();

			uiManager.Render();
			window.SwapBuffersAndPollEvents();
		}

		return 0;
	}
}
