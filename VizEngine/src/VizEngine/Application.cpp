#include "Application.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GUI/UIManager.h"
#include "Core/Camera.h"
#include "Core/Mesh.h"
#include "Core/Transform.h"
#include "Core/Scene.h"
#include "Core/Light.h"
#include "Core/Model.h"
#include "Log.h"

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
		std::shared_ptr<Mesh> planeMesh(Mesh::CreatePlane(20.0f).release());

		// =========================================================================
		// Build Scene
		// =========================================================================
		Scene scene;

		// Add a ground plane
		auto& ground = scene.Add(planeMesh, "Ground");
		ground.ObjectTransform.Position = glm::vec3(0.0f, -1.0f, 0.0f);
		ground.Color = glm::vec4(0.3f, 0.3f, 0.35f, 1.0f);

		// Add a pyramid
		auto& pyramid = scene.Add(pyramidMesh, "Pyramid");
		pyramid.ObjectTransform.Position = glm::vec3(-3.0f, 0.0f, 0.0f);
		pyramid.ObjectTransform.Scale = glm::vec3(2.0f, 4.0f, 2.0f);
		pyramid.Color = glm::vec4(0.3f, 0.5f, 0.9f, 1.0f);

		// Add a cube
		auto& cube = scene.Add(cubeMesh, "Cube");
		cube.ObjectTransform.Position = glm::vec3(3.0f, 0.0f, 0.0f);
		cube.ObjectTransform.Scale = glm::vec3(2.0f);
		cube.Color = glm::vec4(0.9f, 0.5f, 0.3f, 1.0f);

		// =========================================================================
		// Load glTF Model (testing tinygltf)
		// =========================================================================
		auto duckModel = Model::LoadFromFile("assets/gltf-samples/Models/Duck/glTF-Binary/Duck.glb");
		if (duckModel)
		{
			VP_CORE_INFO("Duck model loaded: {} meshes", duckModel->GetMeshCount());
			for (size_t i = 0; i < duckModel->GetMeshCount(); i++)
			{
				auto& duckObj = scene.Add(duckModel->GetMeshes()[i], "Duck");
				duckObj.ObjectTransform.Position = glm::vec3(0.0f, 0.0f, 3.0f);
				duckObj.ObjectTransform.Scale = glm::vec3(0.02f);
				duckObj.Color = glm::vec4(1.0f, 0.9f, 0.0f, 1.0f);
			}
		}
		else
		{
			VP_CORE_ERROR("Failed to load Duck model!");
		}

		// =========================================================================
		// Lighting
		// =========================================================================
		DirectionalLight light;
		light.Direction = glm::vec3(-0.5f, -1.0f, -0.3f);
		light.Ambient = glm::vec3(0.2f, 0.2f, 0.25f);
		light.Diffuse = glm::vec3(0.8f, 0.8f, 0.75f);
		light.Specular = glm::vec3(1.0f, 1.0f, 0.95f);

		// =========================================================================
		// Camera
		// =========================================================================
		Camera camera(45.0f, static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
		camera.SetPosition(glm::vec3(0.0f, 6.0f, -15.0f));

		// =========================================================================
		// Load Assets
		// =========================================================================
		Shader litShader("resources/shaders/lit.shader");
		Texture texture("resources/textures/uvchecker.png");
		texture.Bind();

		// =========================================================================
		// UI & Renderer
		// =========================================================================
		UIManager uiManager(window.GetWindow());
		Renderer renderer;

		// =========================================================================
		// Runtime Variables
		// =========================================================================
		float clearColor[4] = { 0.1f, 0.1f, 0.15f, 1.0f };
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

			// Rotate objects (skip ground plane at index 0)
			for (size_t i = 1; i < scene.Size(); i++)
			{
				auto& obj = scene[i];
				obj.ObjectTransform.Rotation.y += rotationSpeed * deltaTime;
			}

			// --- Set Light Uniforms ---
			litShader.Bind();
			litShader.SetVec3("u_LightDirection", light.GetDirection());
			litShader.SetVec3("u_LightAmbient", light.Ambient);
			litShader.SetVec3("u_LightDiffuse", light.Diffuse);
			litShader.SetVec3("u_LightSpecular", light.Specular);
			litShader.SetVec3("u_ViewPos", camera.GetPosition());

			// --- Render ---
			renderer.Clear(clearColor);
			scene.Render(renderer, litShader, camera);

			// --- UI: Scene Objects Panel ---
			uiManager.StartWindow("Scene Objects");

			// Object list
			ImGui::Text("Objects (%zu)", scene.Size());
			ImGui::Separator();

			for (size_t i = 0; i < scene.Size(); i++)
			{
				bool isSelected = (selectedObject == static_cast<int>(i));
				if (ImGui::Selectable(scene[i].Name.c_str(), isSelected))
				{
					selectedObject = static_cast<int>(i);
				}
			}

			ImGui::Separator();

			// Edit selected object
			if (selectedObject >= 0 && selectedObject < static_cast<int>(scene.Size()))
			{
				auto& obj = scene[static_cast<size_t>(selectedObject)];

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
				ImGui::SliderFloat("Shininess", &obj.Shininess, 1.0f, 256.0f, "%.0f");

				ImGui::Separator();
				if (ImGui::Button("Delete Object"))
				{
					scene.Remove(static_cast<size_t>(selectedObject));
					selectedObject = std::min(selectedObject, static_cast<int>(scene.Size()) - 1);
					if (selectedObject < 0) selectedObject = 0;
				}
			}

			ImGui::Separator();

			// Add new objects
			if (ImGui::Button("Add Pyramid"))
			{
				auto& newObj = scene.Add(pyramidMesh, "Pyramid " + std::to_string(scene.Size() + 1));
				newObj.ObjectTransform.Scale = glm::vec3(2.0f, 4.0f, 2.0f);
				newObj.Color = glm::vec4(0.5f, 0.5f, 0.9f, 1.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Add Cube"))
			{
				auto& newObj = scene.Add(cubeMesh, "Cube " + std::to_string(scene.Size() + 1));
				newObj.ObjectTransform.Scale = glm::vec3(2.0f);
				newObj.Color = glm::vec4(0.9f, 0.5f, 0.3f, 1.0f);
			}

			uiManager.EndWindow();

			// --- UI: Lighting Panel ---
			uiManager.StartWindow("Lighting");

			ImGui::Text("Directional Light");
			ImGui::DragFloat3("Direction", glm::value_ptr(light.Direction), 0.01f, -1.0f, 1.0f);
			ImGui::ColorEdit3("Ambient", glm::value_ptr(light.Ambient));
			ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.Diffuse));
			ImGui::ColorEdit3("Specular", glm::value_ptr(light.Specular));

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
