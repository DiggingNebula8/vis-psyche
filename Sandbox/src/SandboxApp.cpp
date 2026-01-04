#include <VizEngine.h>
#include <VizEngine/Events/ApplicationEvent.h>
#include <VizEngine/Events/KeyEvent.h>

class Sandbox : public VizEngine::Application
{
public:
	void OnCreate() override
	{
		// =========================================================================
		// Create Shared Meshes
		// =========================================================================
		m_PyramidMesh = std::shared_ptr<VizEngine::Mesh>(VizEngine::Mesh::CreatePyramid().release());
		m_CubeMesh = std::shared_ptr<VizEngine::Mesh>(VizEngine::Mesh::CreateCube().release());
		m_PlaneMesh = std::shared_ptr<VizEngine::Mesh>(VizEngine::Mesh::CreatePlane(20.0f).release());

		// =========================================================================
		// Build Scene
		// =========================================================================
		// Add a ground plane
		auto& ground = m_Scene.Add(m_PlaneMesh, "Ground");
		ground.ObjectTransform.Position = glm::vec3(0.0f, -1.0f, 0.0f);
		ground.Color = glm::vec4(0.3f, 0.3f, 0.35f, 1.0f);

		// Add a pyramid
		auto& pyramid = m_Scene.Add(m_PyramidMesh, "Pyramid");
		pyramid.ObjectTransform.Position = glm::vec3(-3.0f, 0.0f, 0.0f);
		pyramid.ObjectTransform.Scale = glm::vec3(2.0f, 4.0f, 2.0f);
		pyramid.Color = glm::vec4(0.3f, 0.5f, 0.9f, 1.0f);

		// Add a cube
		auto& cube = m_Scene.Add(m_CubeMesh, "Cube");
		cube.ObjectTransform.Position = glm::vec3(3.0f, 0.0f, 0.0f);
		cube.ObjectTransform.Scale = glm::vec3(2.0f);
		cube.Color = glm::vec4(0.9f, 0.5f, 0.3f, 1.0f);

		// =========================================================================
		// Load glTF Model
		// =========================================================================
		auto duckModel = VizEngine::Model::LoadFromFile("assets/gltf-samples/Models/Duck/glTF-Binary/Duck.glb");
		if (duckModel)
		{
			VP_INFO("Duck model loaded: {} meshes", duckModel->GetMeshCount());

			// Store mesh and material properties for reuse (Add Duck button)
			if (duckModel->GetMeshCount() > 0)
			{
				m_DuckMesh = duckModel->GetMeshes()[0];
				const auto& material = duckModel->GetMaterialForMesh(0);
				m_DuckColor = material.BaseColor;
				m_DuckRoughness = material.Roughness;
				if (material.BaseColorTexture)
				{
					m_DuckTexture = material.BaseColorTexture;
				}
			}

			// Add initial duck to scene
			for (size_t i = 0; i < duckModel->GetMeshCount(); i++)
			{
				auto& duckObj = m_Scene.Add(duckModel->GetMeshes()[i], "Duck");
				duckObj.ObjectTransform.Position = glm::vec3(0.0f, 0.0f, 3.0f);
				duckObj.ObjectTransform.Scale = glm::vec3(0.02f);

				// Copy material properties from glTF
				const auto& material = duckModel->GetMaterialForMesh(i);
				duckObj.Color = material.BaseColor;
				duckObj.Roughness = material.Roughness;
				if (material.BaseColorTexture)
				{
					duckObj.TexturePtr = material.BaseColorTexture;
				}
			}
		}
		else
		{
			VP_ERROR("Failed to load Duck model!");
		}

		// =========================================================================
		// Lighting
		// =========================================================================
		m_Light.Direction = glm::vec3(-0.5f, -1.0f, -0.3f);
		m_Light.Ambient = glm::vec3(0.2f, 0.2f, 0.25f);
		m_Light.Diffuse = glm::vec3(0.8f, 0.8f, 0.75f);
		m_Light.Specular = glm::vec3(1.0f, 1.0f, 0.95f);

		// =========================================================================
		// Camera
		// =========================================================================
		m_Camera = VizEngine::Camera(45.0f, 800.0f / 800.0f, 0.1f, 100.0f);
		m_Camera.SetPosition(glm::vec3(0.0f, 6.0f, -15.0f));

		// =========================================================================
		// Load Assets
		// =========================================================================
		m_LitShader = std::make_unique<VizEngine::Shader>("resources/shaders/lit.shader");
		m_DefaultTexture = std::make_shared<VizEngine::Texture>("resources/textures/uvchecker.png");

		// Assign default texture to basic objects (created before this point)
		for (size_t i = 0; i < m_Scene.Size(); i++)
		{
			if (!m_Scene[i].TexturePtr)
			{
				m_Scene[i].TexturePtr = m_DefaultTexture;
			}
		}
	}

	void OnUpdate(float deltaTime) override
	{
		// =========================================================================
		// Engine Stats
		// =========================================================================
		m_FrameCount++;
		m_FpsUpdateTimer += deltaTime;
		if (m_FpsUpdateTimer >= 0.5f)
		{
			m_CurrentFPS = 1.0f / deltaTime;
			m_FpsUpdateTimer = 0.0f;
		}

		// =========================================================================
		// Camera Controller
		// =========================================================================
		float speed = m_MoveSpeed * deltaTime;
		if (VizEngine::Input::IsKeyHeld(VizEngine::KeyCode::LeftShift))
			speed *= m_SprintMultiplier;

		// WASD movement
		if (VizEngine::Input::IsKeyHeld(VizEngine::KeyCode::W)) m_Camera.MoveForward(speed);
		if (VizEngine::Input::IsKeyHeld(VizEngine::KeyCode::S)) m_Camera.MoveForward(-speed);
		if (VizEngine::Input::IsKeyHeld(VizEngine::KeyCode::A)) m_Camera.MoveRight(-speed);
		if (VizEngine::Input::IsKeyHeld(VizEngine::KeyCode::D)) m_Camera.MoveRight(speed);
		if (VizEngine::Input::IsKeyHeld(VizEngine::KeyCode::E)) m_Camera.MoveUp(speed);
		if (VizEngine::Input::IsKeyHeld(VizEngine::KeyCode::Q)) m_Camera.MoveUp(-speed);

		// Mouse look (hold right mouse button)
		if (VizEngine::Input::IsMouseButtonHeld(VizEngine::MouseCode::Right))
		{
			glm::vec2 delta = VizEngine::Input::GetMouseDelta();
			float yaw = m_Camera.GetYaw() - delta.x * m_LookSensitivity;
			float pitch = m_Camera.GetPitch() - delta.y * m_LookSensitivity;
			pitch = glm::clamp(pitch, -1.5f, 1.5f);
			m_Camera.SetRotation(pitch, yaw);
		}

		// Scroll zoom
		float scroll = VizEngine::Input::GetScrollDelta();
		if (scroll != 0.0f)
		{
			float fov = m_Camera.GetFOV() - scroll * 2.0f;
			m_Camera.SetFOV(glm::clamp(fov, 10.0f, 90.0f));
		}

		// =========================================================================
		// Object Rotation (skip ground plane by name, not index)
		// =========================================================================
		for (auto& obj : m_Scene)
		{
			if (obj.Name == "Ground") continue;
			obj.ObjectTransform.Rotation.y += m_RotationSpeed * deltaTime;
		}
	}

	void OnRender() override
	{
		auto& engine = VizEngine::Engine::Get();
		auto& renderer = engine.GetRenderer();

		// Set light uniforms
		m_LitShader->Bind();
		m_LitShader->SetVec3("u_LightDirection", m_Light.GetDirection());
		m_LitShader->SetVec3("u_LightAmbient", m_Light.Ambient);
		m_LitShader->SetVec3("u_LightDiffuse", m_Light.Diffuse);
		m_LitShader->SetVec3("u_LightSpecular", m_Light.Specular);
		m_LitShader->SetVec3("u_ViewPos", m_Camera.GetPosition());

		// Clear and render
		renderer.Clear(m_ClearColor);
		m_Scene.Render(renderer, *m_LitShader, m_Camera);
	}

	void OnImGuiRender() override
	{
		auto& engine = VizEngine::Engine::Get();
		auto& uiManager = engine.GetUIManager();

		// =========================================================================
		// Engine Stats Panel (toggle with F1)
		// =========================================================================
		if (m_ShowEngineStats)
		{
			uiManager.StartWindow("Engine Stats");

			uiManager.Text("FPS: %.1f", m_CurrentFPS);
			uiManager.Text("Delta: %.2f ms", engine.GetDeltaTime() * 1000.0f);
			uiManager.Text("Frame: %llu", m_FrameCount);
			uiManager.Separator();
			uiManager.Text("Window: %d x %d", m_WindowWidth, m_WindowHeight);
			uiManager.Separator();
			uiManager.Text("Press F1 to toggle");

			uiManager.EndWindow();
		}

		// =========================================================================
		// Scene Objects Panel
		// =========================================================================
		uiManager.StartWindow("Scene Objects");

		uiManager.Text("Objects (%zu)", m_Scene.Size());
		uiManager.Separator();

		for (size_t i = 0; i < m_Scene.Size(); i++)
		{
			bool isSelected = (m_SelectedObject == static_cast<int>(i));
			if (uiManager.Selectable(m_Scene[i].Name.c_str(), isSelected))
			{
				m_SelectedObject = static_cast<int>(i);
			}
		}

		uiManager.Separator();

		// Edit selected object
		if (m_SelectedObject >= 0 && m_SelectedObject < static_cast<int>(m_Scene.Size()))
		{
			auto& obj = m_Scene[static_cast<size_t>(m_SelectedObject)];

			uiManager.Text("Selected: %s", obj.Name.c_str());
			uiManager.Checkbox("Active", &obj.Active);

			uiManager.Separator();
			uiManager.Text("Transform");
			uiManager.DragFloat3("Position", &obj.ObjectTransform.Position.x, 0.1f);

			glm::vec3 rotDegrees = obj.ObjectTransform.GetRotationDegrees();
			if (uiManager.DragFloat3("Rotation", &rotDegrees.x, 1.0f))
			{
				obj.ObjectTransform.SetRotationDegrees(rotDegrees);
			}

			uiManager.DragFloat3("Scale", &obj.ObjectTransform.Scale.x, 0.1f, 0.1f, 10.0f);

			uiManager.Separator();
			uiManager.Text("Appearance");
			uiManager.ColorEdit4("Color", &obj.Color.x);
			uiManager.SliderFloat("Roughness", &obj.Roughness, 0.0f, 1.0f);

			uiManager.Separator();
			if (uiManager.Button("Delete Object"))
			{
				m_Scene.Remove(static_cast<size_t>(m_SelectedObject));
				m_SelectedObject = std::min(m_SelectedObject, static_cast<int>(m_Scene.Size()) - 1);
				if (m_SelectedObject < 0) m_SelectedObject = 0;
			}
		}

		uiManager.Separator();

		// Add new objects (use monotonic counter for unique names)
		if (uiManager.Button("Add Pyramid"))
		{
			auto& newObj = m_Scene.Add(m_PyramidMesh, "Pyramid_" + std::to_string(m_NextObjectID++));
			newObj.ObjectTransform.Scale = glm::vec3(2.0f, 4.0f, 2.0f);
			newObj.Color = glm::vec4(0.5f, 0.5f, 0.9f, 1.0f);
			newObj.TexturePtr = m_DefaultTexture;
		}
		uiManager.SameLine();
		if (uiManager.Button("Add Cube"))
		{
			auto& newObj = m_Scene.Add(m_CubeMesh, "Cube_" + std::to_string(m_NextObjectID++));
			newObj.ObjectTransform.Scale = glm::vec3(2.0f);
			newObj.Color = glm::vec4(0.9f, 0.5f, 0.3f, 1.0f);
			newObj.TexturePtr = m_DefaultTexture;
		}
		if (m_DuckMesh)
		{
			uiManager.SameLine();
			if (uiManager.Button("Add Duck"))
			{
				auto& newObj = m_Scene.Add(m_DuckMesh, "Duck_" + std::to_string(m_NextObjectID++));
				newObj.ObjectTransform.Scale = glm::vec3(0.02f);
				newObj.Color = m_DuckColor;
				newObj.Roughness = m_DuckRoughness;
				newObj.TexturePtr = m_DuckTexture;
			}
		}

		uiManager.EndWindow();

		// =========================================================================
		// Lighting Panel
		// =========================================================================
		uiManager.StartWindow("Lighting");

		uiManager.Text("Directional Light");
		uiManager.DragFloat3("Direction", &m_Light.Direction.x, 0.01f, -1.0f, 1.0f);
		uiManager.ColorEdit3("Ambient", &m_Light.Ambient.x);
		uiManager.ColorEdit3("Diffuse", &m_Light.Diffuse.x);
		uiManager.ColorEdit3("Specular", &m_Light.Specular.x);

		uiManager.EndWindow();

		// =========================================================================
		// Scene Controls Panel
		// =========================================================================
		uiManager.StartWindow("Scene Controls");

		uiManager.Text("Background");
		uiManager.ColorEdit4("Clear Color", m_ClearColor);
		uiManager.Separator();

		uiManager.Text("Animation");
		uiManager.SliderFloat("Rotation Speed", &m_RotationSpeed, 0.0f, 5.0f);
		uiManager.Separator();

		uiManager.Text("Camera");
		glm::vec3 camPos = m_Camera.GetPosition();
		if (uiManager.DragFloat3("Camera Pos", &camPos.x, 0.1f))
		{
			m_Camera.SetPosition(camPos);
		}

		uiManager.EndWindow();
	}

	void OnEvent(VizEngine::Event& e) override
	{
		VizEngine::EventDispatcher dispatcher(e);

		// Handle window resize - update camera and track dimensions
		dispatcher.Dispatch<VizEngine::WindowResizeEvent>(
			[this](VizEngine::WindowResizeEvent& event) {
				m_WindowWidth = event.GetWidth();
				m_WindowHeight = event.GetHeight();

				if (m_WindowWidth > 0 && m_WindowHeight > 0)
				{
					float aspect = static_cast<float>(m_WindowWidth)
					             / static_cast<float>(m_WindowHeight);
					m_Camera.SetAspectRatio(aspect);
				}
				return false;  // Don't consume, allow propagation
			}
		);

		// F1 toggles Engine Stats panel
		dispatcher.Dispatch<VizEngine::KeyPressedEvent>(
			[this](VizEngine::KeyPressedEvent& event) {
				if (event.GetKeyCode() == VizEngine::KeyCode::F1 && !event.IsRepeat())
				{
					m_ShowEngineStats = !m_ShowEngineStats;
					VP_INFO("Engine Stats: {}", m_ShowEngineStats ? "ON" : "OFF");
					return true;  // Consumed
				}
				return false;
			}
		);
	}

	void OnDestroy() override
	{
		// RAII handles cleanup
	}

private:
	// Scene
	VizEngine::Scene m_Scene;
	VizEngine::Camera m_Camera;
	VizEngine::DirectionalLight m_Light;

	// Assets
	std::unique_ptr<VizEngine::Shader> m_LitShader;
	std::shared_ptr<VizEngine::Texture> m_DefaultTexture;
	std::shared_ptr<VizEngine::Mesh> m_PyramidMesh;
	std::shared_ptr<VizEngine::Mesh> m_CubeMesh;
	std::shared_ptr<VizEngine::Mesh> m_PlaneMesh;

	// Duck model assets (for spawning)
	std::shared_ptr<VizEngine::Mesh> m_DuckMesh;
	std::shared_ptr<VizEngine::Texture> m_DuckTexture;
	glm::vec4 m_DuckColor = glm::vec4(1.0f);
	float m_DuckRoughness = 0.5f;

	// Runtime state
	float m_ClearColor[4] = { 0.1f, 0.1f, 0.15f, 1.0f };
	float m_RotationSpeed = 0.5f;
	int m_SelectedObject = 0;
	uint32_t m_NextObjectID = 1;  // Monotonic counter for unique object names

	// Camera controller settings
	float m_MoveSpeed = 5.0f;
	float m_SprintMultiplier = 2.5f;
	float m_LookSensitivity = 0.003f;

	// Engine stats
	bool m_ShowEngineStats = true;
	uint64_t m_FrameCount = 0;
	float m_FpsUpdateTimer = 0.0f;
	float m_CurrentFPS = 0.0f;
	int m_WindowWidth = 800;
	int m_WindowHeight = 800;
};

std::unique_ptr<VizEngine::Application> VizEngine::CreateApplication(VizEngine::EngineConfig& config)
{
	config.Title = "Sandbox - VizPsyche";
	config.Width = 800;
	config.Height = 800;
	return std::make_unique<Sandbox>();
}
