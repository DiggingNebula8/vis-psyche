#include "Application.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GUI/UIManager.h"
#include "Core/Camera.h"
#include "Core/Mesh.h"
#include "Core/Transform.h"
#include "Core/Scene.h"

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
		// Create Shared Meshes
		// =========================================================================
		// Move unique_ptr to shared_ptr for shared ownership across objects
		std::shared_ptr<Mesh> pyramidMesh(Mesh::CreatePyramid().release());
		std::shared_ptr<Mesh> cubeMesh(Mesh::CreateCube().release());

		// =========================================================================
		// Build Scene
		// =========================================================================
		Scene scene;

		// Add a row of pyramids
		for (int i = 0; i < 5; i++)
		{
			auto& obj = scene.AddObject(pyramidMesh, "Pyramid " + std::to_string(i + 1));
			obj.ObjectTransform.Position = glm::vec3((i - 2) * 4.0f, 0.0f, 0.0f);
			obj.ObjectTransform.Scale = glm::vec3(2.0f, 4.0f, 2.0f);
			// Gradient colors
			obj.Color = glm::vec4(0.2f + i * 0.15f, 0.3f, 0.8f - i * 0.1f, 1.0f);
		}

		// Add a cube
		auto& cube = scene.AddObject(cubeMesh, "Cube");
		cube.ObjectTransform.Position = glm::vec3(0.0f, 3.0f, -5.0f);
		cube.ObjectTransform.Scale = glm::vec3(2.0f);
		cube.Color = glm::vec4(0.8f, 0.4f, 0.2f, 1.0f);

		// =========================================================================
		// Camera
		// =========================================================================
		Camera camera(45.0f, static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
		camera.SetPosition(glm::vec3(0.0f, 6.0f, -20.0f));

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
		float rotationSpeed = 0.5f;
		double prevTime = glfwGetTime();
		int selectedObject = 0;

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

			// Rotate all pyramids
			for (size_t i = 0; i < scene.GetObjectCount(); i++)
			{
				auto& obj = scene.GetObject(i);
				if (obj.Name.find("Pyramid") != std::string::npos)
				{
					obj.ObjectTransform.Rotation.y += rotationSpeed * deltaTime * (1.0f + i * 0.2f);
				}
			}

			// --- Render ---
			renderer.Clear(clearColor);
			scene.Render(renderer, shader, camera);

			// --- UI: Scene Objects Panel ---
			uiManager.StartWindow("Scene Objects");

			// Object list
			auto& objects = scene.GetObjects();
			ImGui::Text("Objects (%zu)", objects.size());
			ImGui::Separator();

			for (size_t i = 0; i < objects.size(); i++)
			{
				bool isSelected = (selectedObject == static_cast<int>(i));
				if (ImGui::Selectable(objects[i].Name.c_str(), isSelected))
				{
					selectedObject = static_cast<int>(i);
				}
			}

			ImGui::Separator();

			// Edit selected object
			if (selectedObject >= 0 && selectedObject < static_cast<int>(objects.size()))
			{
				auto& obj = objects[static_cast<size_t>(selectedObject)];

				ImGui::Text("Selected: %s", obj.Name.c_str());
				ImGui::Checkbox("Active", &obj.Active);
				
				ImGui::Separator();
				ImGui::Text("Transform");
				ImGui::DragFloat3("Position", glm::value_ptr(obj.ObjectTransform.Position), 0.1f);
				
				glm::vec3 rotDegrees = obj.ObjectTransform.GetRotationDegrees();
				if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotDegrees), 1.0f))
				{
					obj.ObjectTransform.SetRotationDegrees(rotDegrees);
				}
				
				ImGui::DragFloat3("Scale", glm::value_ptr(obj.ObjectTransform.Scale), 0.1f, 0.1f, 10.0f);

				ImGui::Separator();
				ImGui::Text("Appearance");
				ImGui::ColorEdit4("Color", glm::value_ptr(obj.Color));

				ImGui::Separator();
				if (ImGui::Button("Delete Object"))
				{
					scene.RemoveObject(static_cast<size_t>(selectedObject));
					selectedObject = std::min(selectedObject, static_cast<int>(scene.GetObjectCount()) - 1);
					if (selectedObject < 0) selectedObject = 0;
				}
			}

			ImGui::Separator();

			// Add new objects
			if (ImGui::Button("Add Pyramid"))
			{
				auto& newObj = scene.AddObject(pyramidMesh, "Pyramid " + std::to_string(scene.GetObjectCount() + 1));
				newObj.ObjectTransform.Scale = glm::vec3(2.0f, 4.0f, 2.0f);
				newObj.Color = glm::vec4(0.5f, 0.5f, 0.9f, 1.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Add Cube"))
			{
				auto& newObj = scene.AddObject(cubeMesh, "Cube " + std::to_string(scene.GetObjectCount() + 1));
				newObj.ObjectTransform.Scale = glm::vec3(2.0f);
				newObj.Color = glm::vec4(0.9f, 0.5f, 0.3f, 1.0f);
			}

			uiManager.EndWindow();

			// --- UI: Scene Controls Panel ---
			uiManager.StartWindow("Scene Controls");

			ImGui::Text("Background");
			ImGui::ColorEdit4("Clear Color", clearColor);
			ImGui::Separator();

			ImGui::Text("Animation");
			ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 5.0f);
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
