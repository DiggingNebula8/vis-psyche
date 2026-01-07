#include <VizEngine.h>
#include <VizEngine/Events/ApplicationEvent.h>
#include <VizEngine/Events/KeyEvent.h>
#include <VizEngine/Renderer/Bloom.h>
#include <chrono>

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
		m_ShadowDepthShader = std::make_unique<VizEngine::Shader>("resources/shaders/shadow_depth.shader");
		m_DefaultTexture = std::make_shared<VizEngine::Texture>("resources/textures/uvchecker.png");

		// Assign default texture to basic objects (created before this point)
		for (size_t i = 0; i < m_Scene.Size(); i++)
		{
			if (!m_Scene[i].TexturePtr)
			{
				m_Scene[i].TexturePtr = m_DefaultTexture;
			}
		}

		// =========================================================================
		// Create Framebuffer for offscreen rendering
		// =========================================================================
		int fbWidth = 800;
		int fbHeight = 800;

		// Create color attachment (RGBA8)
		m_FramebufferColor = std::make_shared<VizEngine::Texture>(
			fbWidth, fbHeight,
			GL_RGBA8,           // Internal format
			GL_RGBA,            // Format
			GL_UNSIGNED_BYTE    // Data type
		);

		// Create depth attachment (Depth24)
		m_FramebufferDepth = std::make_shared<VizEngine::Texture>(
			fbWidth, fbHeight,
			GL_DEPTH_COMPONENT24,   // Internal format
			GL_DEPTH_COMPONENT,     // Format
			GL_FLOAT                // Data type
		);

		// Create framebuffer and attach textures
		m_Framebuffer = std::make_shared<VizEngine::Framebuffer>(fbWidth, fbHeight);
		m_Framebuffer->AttachColorTexture(m_FramebufferColor, 0);
		m_Framebuffer->AttachDepthTexture(m_FramebufferDepth);

		// Verify framebuffer is complete
		if (!m_Framebuffer->IsComplete())
		{
			VP_ERROR("Framebuffer is not complete! Disabling offscreen render.");
			m_Framebuffer.reset();
			m_ShowFramebufferTexture = false;
		}
		else
		{
			VP_INFO("Framebuffer created successfully: {}x{}", fbWidth, fbHeight);
		}

		// =========================================================================
		// Create Shadow Map Framebuffer (depth-only, for shadow rendering)
		// =========================================================================
		int shadowMapResolution = 2048;  // Higher = crisper shadows (1024, 2048, 4096)

		// Create depth texture for shadow map
		m_ShadowMapDepth = std::make_shared<VizEngine::Texture>(
			shadowMapResolution, shadowMapResolution,
			GL_DEPTH_COMPONENT24,   // Internal format (24-bit depth)
			GL_DEPTH_COMPONENT,     // Format
			GL_FLOAT                // Data type
		);
		
		// Configure shadow map texture for correct sampling
		m_ShadowMapDepth->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_ShadowMapDepth->SetBorderColor(borderColor);

		// Create framebuffer and attach depth texture only (no color)
		m_ShadowMapFramebuffer = std::make_shared<VizEngine::Framebuffer>(
			shadowMapResolution, shadowMapResolution
		);
		m_ShadowMapFramebuffer->AttachDepthTexture(m_ShadowMapDepth);

		// Verify shadow map framebuffer is complete
		if (!m_ShadowMapFramebuffer->IsComplete())
		{
			VP_ERROR("Shadow map framebuffer is not complete! Disabling shadows.");
			m_ShadowMapFramebuffer.reset();
			// Also reset depth texture to avoid binding invalid texture
			m_ShadowMapDepth.reset();
			m_ShowShadowMap = false;
		}
		else
		{
			VP_INFO("Shadow map framebuffer created: {}x{}", shadowMapResolution, shadowMapResolution);
		}

		// =========================================================================
		// Create Skybox from HDRI
		// =========================================================================
		VP_INFO("Loading environment HDRI...");

		// Load HDR equirectangular map
		m_EnvironmentHDRI = std::make_shared<VizEngine::Texture>(
			"resources/textures/environments/qwantani_dusk_2_puresky_2k.hdr", 
			true  // isHDR
		);

		// Convert to cubemap (one-time operation)
		int cubemapResolution = 512;  // 512x512 per face
		m_SkyboxCubemap = VizEngine::CubemapUtils::EquirectangularToCubemap(
			m_EnvironmentHDRI, 
			cubemapResolution
		);

		// Release original HDRI to free memory (~6MB for 2K texture)
		// The cubemap now contains all the data we need
		m_EnvironmentHDRI.reset();

		// Create skybox
		m_Skybox = std::make_unique<VizEngine::Skybox>(m_SkyboxCubemap);

		VP_INFO("Skybox ready!");

		// =========================================================================
		// Generate IBL Maps (Chapter 34)
		// =========================================================================
		auto iblStart = std::chrono::high_resolution_clock::now();

		m_IrradianceMap = VizEngine::CubemapUtils::GenerateIrradianceMap(m_SkyboxCubemap, 32);
		m_PrefilteredMap = VizEngine::CubemapUtils::GeneratePrefilteredMap(m_SkyboxCubemap, 512);
		m_BRDFLut = VizEngine::CubemapUtils::GenerateBRDFLUT(512);

		// Validate IBL maps and disable IBL if any failed
		if (!m_IrradianceMap)
		{
			VP_ERROR("Failed to generate irradiance map - IBL disabled");
			m_UseIBL = false;
		}
		if (!m_PrefilteredMap)
		{
			VP_ERROR("Failed to generate prefiltered environment map - IBL disabled");
			m_UseIBL = false;
		}
		if (!m_BRDFLut)
		{
			VP_ERROR("Failed to generate BRDF LUT - IBL disabled");
			m_UseIBL = false;
		}

		auto iblEnd = std::chrono::high_resolution_clock::now();
		auto iblDuration = std::chrono::duration_cast<std::chrono::milliseconds>(iblEnd - iblStart);
		if (m_UseIBL)
		{
			VP_INFO("IBL maps generated in {}ms", iblDuration.count());
		}

		// =========================================================================
		// PBR Rendering Setup (Chapter 33)
		// =========================================================================
		m_DefaultLitShader = std::make_unique<VizEngine::Shader>("resources/shaders/defaultlit.shader");
		m_SphereMesh = std::shared_ptr<VizEngine::Mesh>(VizEngine::Mesh::CreateSphere(1.0f, 32).release());
		VP_INFO("PBR rendering initialized");

		// =========================================================================
		// HDR Pipeline Setup (Chapter 35)
		// =========================================================================
		VP_INFO("Setting up HDR pipeline...");

		// Create HDR color texture (RGB16F)
		m_HDRColorTexture = std::make_shared<VizEngine::Texture>(
			m_WindowWidth, m_WindowHeight,
			GL_RGB16F,           // Internal format (HDR)
			GL_RGB,              // Format
			GL_FLOAT             // Data type
		);

		// Create depth texture
		m_HDRDepthTexture = std::make_shared<VizEngine::Texture>(
			m_WindowWidth, m_WindowHeight,
			GL_DEPTH_COMPONENT24,
			GL_DEPTH_COMPONENT,
			GL_FLOAT
		);

		// Create HDR framebuffer and attach textures
		m_HDRFramebuffer = std::make_shared<VizEngine::Framebuffer>(m_WindowWidth, m_WindowHeight);
		m_HDRFramebuffer->AttachColorTexture(m_HDRColorTexture, 0);
		m_HDRFramebuffer->AttachDepthTexture(m_HDRDepthTexture);

		// Verify framebuffer is complete
		if (!m_HDRFramebuffer->IsComplete())
		{
			VP_ERROR("HDR Framebuffer is not complete!");
		}
		else
		{
			VP_INFO("HDR Framebuffer created successfully: {}x{} (RGB16F)", 
			        m_WindowWidth, m_WindowHeight);
		}

		// Load tone mapping shader
		m_ToneMappingShader = std::make_shared<VizEngine::Shader>("resources/shaders/tonemapping.shader");
		if (!m_ToneMappingShader->IsValid())
		{
			VP_ERROR("Failed to load tone mapping shader!");
		}

		// Create fullscreen quad
		m_FullscreenQuad = std::make_shared<VizEngine::FullscreenQuad>();

		VP_INFO("HDR pipeline initialized successfully");

		// =========================================================================
		// Post-Processing Setup (Chapter 36)
		// =========================================================================
		VP_INFO("Setting up post-processing...");

		// Create Bloom Processor (half resolution for performance)
		int bloomWidth = m_WindowWidth / 2;
		int bloomHeight = m_WindowHeight / 2;
		m_Bloom = std::make_unique<VizEngine::Bloom>(bloomWidth, bloomHeight);
		m_Bloom->SetThreshold(m_BloomThreshold);
		m_Bloom->SetKnee(m_BloomKnee);
		m_Bloom->SetBlurPasses(m_BloomBlurPasses);

		VP_INFO("Bloom initialized: {}x{}", bloomWidth, bloomHeight);

		// Create Neutral Color Grading LUT (16x16x16)
		m_ColorGradingLUT = VizEngine::Texture::CreateNeutralLUT3D(16);

		if (m_ColorGradingLUT == 0)
		{
			VP_ERROR("Failed to create color grading LUT!");
		}

		VP_INFO("Post-processing initialized successfully");
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

		// =========================================================================
		// Compute Light-Space Matrix (once per frame)
		// =========================================================================
		m_LightSpaceMatrix = ComputeLightSpaceMatrix(m_Light);

		// =========================================================================
		// Pass 1: Render scene from light's perspective to shadow map
		// =========================================================================
		if (m_ShadowMapFramebuffer && m_ShadowDepthShader)
		{
			m_ShadowMapFramebuffer->Bind();
			renderer.SetViewport(0, 0, m_ShadowMapFramebuffer->GetWidth(), m_ShadowMapFramebuffer->GetHeight());
			renderer.ClearDepth();  // Clear depth buffer (no color attachment)

			// Enable polygon offset to reduce shadow acne
			renderer.EnablePolygonOffset(2.0f, 4.0f);

			// Use shadow depth shader
			m_ShadowDepthShader->Bind();
			m_ShadowDepthShader->SetMatrix4fv("u_LightSpaceMatrix", m_LightSpaceMatrix);

			// Render scene geometry (only need depth, no lighting)
			// We need to set u_Model for each object since Scene::Render uses u_MVP
			for (auto& obj : m_Scene)
			{
				if (!obj.Active || !obj.MeshPtr) continue;

				glm::mat4 model = obj.ObjectTransform.GetModelMatrix();
				m_ShadowDepthShader->SetMatrix4fv("u_Model", model);

				obj.MeshPtr->Bind();
				renderer.Draw(obj.MeshPtr->GetVertexArray(), obj.MeshPtr->GetIndexBuffer(), *m_ShadowDepthShader);
			}

			// Disable polygon offset
			renderer.DisablePolygonOffset();

			m_ShadowMapFramebuffer->Unbind();
		}
		renderer.SetViewport(0, 0, m_WindowWidth, m_WindowHeight);

		// =========================================================================
		// Pass 2: Render scene with PBR to HDR Framebuffer (Chapter 35)
		// =========================================================================
		m_HDRFramebuffer->Bind();
		renderer.Clear(m_ClearColor);

		m_DefaultLitShader->Bind();

		// Set camera matrices
		m_DefaultLitShader->SetMatrix4fv("u_View", m_Camera.GetViewMatrix());
		m_DefaultLitShader->SetMatrix4fv("u_Projection", m_Camera.GetProjectionMatrix());
		m_DefaultLitShader->SetVec3("u_ViewPos", m_Camera.GetPosition());

		// Set PBR point lights
		m_DefaultLitShader->SetInt("u_LightCount", 4);
		for (int i = 0; i < 4; ++i)
		{
			m_DefaultLitShader->SetVec3("u_LightPositions[" + std::to_string(i) + "]", m_PBRLightPositions[i]);
			m_DefaultLitShader->SetVec3("u_LightColors[" + std::to_string(i) + "]", m_PBRLightColors[i]);
		}

		// Set directional light from ImGui Lighting panel
		m_DefaultLitShader->SetBool("u_UseDirLight", true);
		m_DefaultLitShader->SetVec3("u_DirLightDirection", m_Light.GetDirection());
		m_DefaultLitShader->SetVec3("u_DirLightColor", m_Light.Diffuse * 2.0f);

		// Set shadow mapping uniforms
		m_DefaultLitShader->SetMatrix4fv("u_LightSpaceMatrix", m_LightSpaceMatrix);
		if (m_ShadowMapDepth)
		{
			m_ShadowMapDepth->Bind(1);
			m_DefaultLitShader->SetInt("u_ShadowMap", 1);
		}

		// Bind IBL textures (Chapter 34)
		if (m_UseIBL && m_IrradianceMap && m_PrefilteredMap && m_BRDFLut)
		{
			m_IrradianceMap->Bind(5);
			m_DefaultLitShader->SetInt("u_IrradianceMap", 5);

			m_PrefilteredMap->Bind(6);
			m_DefaultLitShader->SetInt("u_PrefilteredMap", 6);

			m_BRDFLut->Bind(7);
			m_DefaultLitShader->SetInt("u_BRDF_LUT", 7);

			m_DefaultLitShader->SetFloat("u_MaxReflectionLOD", 4.0f);  // maxMipLevels - 1
			m_DefaultLitShader->SetBool("u_UseIBL", true);
		}
		else
		{
			m_DefaultLitShader->SetBool("u_UseIBL", false);
		}

		// Render scene objects with PBR
		RenderSceneObjects();

		// =========================================================================
		// Render Skybox to HDR Buffer
		// =========================================================================
		if (m_ShowSkybox && m_Skybox)
		{
			m_Skybox->Render(m_Camera);
		}

		m_HDRFramebuffer->Unbind();

		// =========================================================================
		// Pass 3: Bloom Processing (Chapter 36)
		// =========================================================================
		std::shared_ptr<VizEngine::Texture> bloomTexture = nullptr;
		if (m_EnableBloom && m_Bloom)
		{
			// Update bloom parameters (in case they changed via ImGui)
			m_Bloom->SetThreshold(m_BloomThreshold);
			m_Bloom->SetKnee(m_BloomKnee);
			m_Bloom->SetBlurPasses(m_BloomBlurPasses);

			// Process HDR buffer to generate bloom
			bloomTexture = m_Bloom->Process(m_HDRColorTexture);
		}

		// =========================================================================
		// Pass 4: Tone Mapping + Post-Processing to Screen (Chapter 35 & 36)
		// =========================================================================
		renderer.SetViewport(0, 0, m_WindowWidth, m_WindowHeight);
		renderer.Clear(m_ClearColor);

		// Disable depth test for fullscreen quad
		renderer.DisableDepthTest();

		// Bind tone mapping shader
		m_ToneMappingShader->Bind();

		// Bind HDR texture
		m_HDRColorTexture->Bind(0);
		m_ToneMappingShader->SetInt("u_HDRBuffer", 0);

		// Set tone mapping parameters
		m_ToneMappingShader->SetInt("u_ToneMappingMode", m_ToneMappingMode);
		m_ToneMappingShader->SetFloat("u_Exposure", m_Exposure);
		m_ToneMappingShader->SetFloat("u_Gamma", m_Gamma);
		m_ToneMappingShader->SetFloat("u_WhitePoint", m_WhitePoint);

		// Bloom parameters
		m_ToneMappingShader->SetBool("u_EnableBloom", m_EnableBloom);
		m_ToneMappingShader->SetFloat("u_BloomIntensity", m_BloomIntensity);
		if (bloomTexture)
		{
			bloomTexture->Bind(1);
			m_ToneMappingShader->SetInt("u_BloomTexture", 1);
		}

		// Color grading parameters
		m_ToneMappingShader->SetBool("u_EnableColorGrading", m_EnableColorGrading);
		m_ToneMappingShader->SetFloat("u_LUTContribution", m_LUTContribution);
		m_ToneMappingShader->SetFloat("u_Saturation", m_Saturation);
		m_ToneMappingShader->SetFloat("u_Contrast", m_Contrast);
		m_ToneMappingShader->SetFloat("u_Brightness", m_Brightness);

		if (m_EnableColorGrading && m_ColorGradingLUT != 0)
		{
			VizEngine::Texture::BindTexture3D(m_ColorGradingLUT, 2);
			m_ToneMappingShader->SetInt("u_ColorGradingLUT", 2);
		}

		// Render fullscreen quad
		m_FullscreenQuad->Render();

		// Re-enable depth test
		renderer.EnableDepthTest();

		// =========================================================================
		// Render to preview Framebuffer (offscreen) - kept for F2 preview
		// =========================================================================
		if (m_Framebuffer)
		{
			float windowAspect = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
			m_Camera.SetAspectRatio(1.0f);  // Framebuffer is square (800x800)
			
			m_Framebuffer->Bind();
			renderer.SetViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
			renderer.Clear(m_ClearColor);

			// PBR shader already configured, just need to update camera for new aspect
			m_DefaultLitShader->Bind();
			m_DefaultLitShader->SetMatrix4fv("u_View", m_Camera.GetViewMatrix());
			m_DefaultLitShader->SetMatrix4fv("u_Projection", m_Camera.GetProjectionMatrix());

			// Render scene objects with PBR
			RenderSceneObjects();
		
			// Render Skybox to offscreen framebuffer
			if (m_ShowSkybox && m_Skybox)
			{
				m_Skybox->Render(m_Camera);
			}
		
			m_Framebuffer->Unbind();

			// Restore camera to window aspect ratio
			m_Camera.SetAspectRatio(windowAspect);
			
			// Restore viewport to window size
			renderer.SetViewport(0, 0, m_WindowWidth, m_WindowHeight);
		}
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
		// Framebuffer Texture Preview (toggle with F2)
		// =========================================================================
		if (m_ShowFramebufferTexture)
		{
			uiManager.StartFixedWindow("Offscreen Render", 360.0f, 420.0f);

			if (m_FramebufferColor && m_Framebuffer)
			{
				// ImGui::Image takes texture ID, size
				unsigned int texID = m_FramebufferColor->GetID();
				float width = static_cast<float>(m_Framebuffer->GetWidth());
				float height = static_cast<float>(m_Framebuffer->GetHeight());

				// Display with fixed size (scale down if needed)
				float displaySize = 320.0f;  // Preview size
				float aspect = width / height;
				uiManager.Image(
					reinterpret_cast<void*>(static_cast<uintptr_t>(texID)),
					displaySize,
					displaySize / aspect
				);

				uiManager.Separator();
				uiManager.Text("Framebuffer: %dx%d", m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
			}
			else
			{
				uiManager.Text("Framebuffer not available");
			}
			
			uiManager.Checkbox("Show Preview", &m_ShowFramebufferTexture);

			uiManager.EndWindow();
		}

		// =========================================================================
		// Shadow Map Preview (toggle with F3)
		// =========================================================================
		if (m_ShowShadowMap)
		{
			uiManager.StartFixedWindow("Shadow Map Debug", 360.0f, 420.0f);

			if (m_ShadowMapDepth && m_ShadowMapFramebuffer)
			{
				unsigned int shadowTexID = m_ShadowMapDepth->GetID();
				float displaySize = 320.0f;

				uiManager.Image(
					reinterpret_cast<void*>(static_cast<uintptr_t>(shadowTexID)),
					displaySize,
					displaySize
				);

				uiManager.Separator();
				uiManager.Text("Shadow Map: %dx%d",
					m_ShadowMapFramebuffer->GetWidth(),
					m_ShadowMapFramebuffer->GetHeight()
				);
			}
			else
			{
				uiManager.Text("Shadow map not available");
			}

			uiManager.Checkbox("Show Shadow Map", &m_ShowShadowMap);

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
			uiManager.Text("Material");
			uiManager.ColorEdit4("Color", &obj.Color.x);
			uiManager.SliderFloat("Roughness", &obj.Roughness, 0.05f, 1.0f);
			uiManager.SliderFloat("Metallic", &obj.Metallic, 0.0f, 1.0f);

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
		if (m_SphereMesh)
		{
			uiManager.SameLine();
			if (uiManager.Button("Add Sphere"))
			{
				auto& newObj = m_Scene.Add(m_SphereMesh, "Sphere_" + std::to_string(m_NextObjectID++));
				newObj.ObjectTransform.Scale = glm::vec3(1.0f);
				newObj.Color = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f);  // Red
				newObj.Metallic = 0.5f;   // Semi-metallic for PBR demo
				newObj.Roughness = 0.3f;  // Somewhat shiny
			}
		}

		uiManager.EndWindow();

		// =========================================================================
		// Lighting Panel
		// =========================================================================
		uiManager.StartWindow("Lighting");

		uiManager.Text("Directional Light");
		uiManager.DragFloat3("Direction", &m_Light.Direction.x, 0.01f, -1.0f, 1.0f);
		uiManager.ColorEdit3("Dir Color", &m_Light.Diffuse.x);

		uiManager.Separator();
		uiManager.Text("Point Lights (4x)");
		
		// Light intensity control (all lights share same intensity)
		if (uiManager.SliderFloat("Intensity", &m_PBRLightIntensity, 50.0f, 1000.0f))
		{
			for (int i = 0; i < 4; ++i)
			{
				m_PBRLightColors[i] = m_PBRLightColor * m_PBRLightIntensity;
			}
		}

		// Light color (applied to all lights)
		if (uiManager.ColorEdit3("Point Color", &m_PBRLightColor.x))
		{
			for (int i = 0; i < 4; ++i)
			{
				m_PBRLightColors[i] = m_PBRLightColor * m_PBRLightIntensity;
			}
		}

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

		// =========================================================================
		// Skybox Controls
		// =========================================================================
		uiManager.StartWindow("Skybox");
		uiManager.Checkbox("Show Skybox", &m_ShowSkybox);
		
		if (m_SkyboxCubemap)
		{
			uiManager.Text("Cubemap: %dx%d per face", 
				m_SkyboxCubemap->GetWidth(), 
				m_SkyboxCubemap->GetHeight());
		}
		else
		{
			uiManager.Text("Cubemap: Not loaded");
		}

		uiManager.EndWindow();

		// =========================================================================
		// IBL Controls (Chapter 34)
		// =========================================================================
		uiManager.StartWindow("IBL");
		uiManager.Checkbox("Use IBL", &m_UseIBL);

		if (m_IrradianceMap && m_PrefilteredMap && m_BRDFLut)
		{
			uiManager.Text("Irradiance: 32x32 cubemap");
			uiManager.Text("Prefiltered: 512x512 cubemap (5 mips)");
			uiManager.Text("BRDF LUT: 512x512 RG texture");
		}
		else
		{
			uiManager.Text("IBL maps not generated!");
		}
		uiManager.EndWindow();

		// =========================================================================
		// HDR & Tone Mapping Panel (Chapter 35)
		// =========================================================================
		uiManager.StartWindow("HDR & Tone Mapping");

		// Tone mapping operator selection
		const char* toneMappingModes[] = { 
			"Reinhard", 
			"Reinhard Extended", 
			"Exposure", 
			"ACES Filmic", 
			"Uncharted 2" 
		};
		uiManager.Combo("Tone Mapping", &m_ToneMappingMode, toneMappingModes, 5);

		// Exposure control (for all modes except simple Reinhard)
		if (m_ToneMappingMode != 0)
		{
			uiManager.SliderFloat("Exposure", &m_Exposure, 0.1f, 5.0f);
			
			// Show f-stop equivalent
			float fStops = log2f(m_Exposure);
			uiManager.Text("(%.2f f-stops)", fStops);
		}

		// White point (for Reinhard Extended)
		if (m_ToneMappingMode == 1)
		{
			uiManager.SliderFloat("White Point", &m_WhitePoint, 1.0f, 20.0f);
		}

		// Gamma control
		uiManager.SliderFloat("Gamma", &m_Gamma, 1.8f, 2.6f);

		uiManager.Separator();

		// Framebuffer info
		if (m_HDRFramebuffer)
		{
			uiManager.Text("HDR Buffer: %dx%d RGB16F", 
			               m_HDRFramebuffer->GetWidth(), 
			               m_HDRFramebuffer->GetHeight());
			uiManager.Text("Memory: ~%.2f MB", 
			               (m_HDRFramebuffer->GetWidth() * m_HDRFramebuffer->GetHeight() * 6) / (1024.0f * 1024.0f));
		}

		uiManager.EndWindow();

		// =========================================================================
		// Post-Processing Panel (Chapter 36)
		// =========================================================================
		uiManager.StartWindow("Post-Processing");

		// Bloom section
		if (uiManager.CollapsingHeader("Bloom"))
		{
			uiManager.Checkbox("Enable Bloom", &m_EnableBloom);
			uiManager.SliderFloat("Threshold", &m_BloomThreshold, 0.0f, 5.0f);
			uiManager.SliderFloat("Knee", &m_BloomKnee, 0.0f, 1.0f);
			uiManager.SliderFloat("Intensity", &m_BloomIntensity, 0.0f, 0.2f);
			uiManager.SliderInt("Blur Passes", &m_BloomBlurPasses, 1, 10);
		}

		// Color Grading section
		if (uiManager.CollapsingHeader("Color Grading"))
		{
			uiManager.Checkbox("Enable Color Grading", &m_EnableColorGrading);
			uiManager.SliderFloat("LUT Contribution", &m_LUTContribution, 0.0f, 1.0f);
			uiManager.Separator();
			uiManager.Text("Parametric Controls");
			uiManager.SliderFloat("Saturation", &m_Saturation, 0.0f, 2.0f);
			uiManager.SliderFloat("Contrast", &m_Contrast, 0.5f, 2.0f);
			uiManager.SliderFloat("Brightness", &m_Brightness, -0.5f, 0.5f);
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

					// Recreate HDR framebuffer with new dimensions (Chapter 35)
					if (m_HDRFramebuffer)
					{
						VP_INFO("Recreating HDR framebuffer: {}x{}", m_WindowWidth, m_WindowHeight);

						m_HDRColorTexture = std::make_shared<VizEngine::Texture>(
							m_WindowWidth, m_WindowHeight, GL_RGB16F, GL_RGB, GL_FLOAT
						);
						m_HDRDepthTexture = std::make_shared<VizEngine::Texture>(
							m_WindowWidth, m_WindowHeight, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT
						);

						m_HDRFramebuffer = std::make_shared<VizEngine::Framebuffer>(m_WindowWidth, m_WindowHeight);
						m_HDRFramebuffer->AttachColorTexture(m_HDRColorTexture, 0);
						m_HDRFramebuffer->AttachDepthTexture(m_HDRDepthTexture);

						if (!m_HDRFramebuffer->IsComplete())
						{
							VP_ERROR("HDR Framebuffer incomplete after resize!");
						}
					}

					// Recreate Bloom processor with new dimensions (Chapter 36)
					if (m_Bloom)
					{
						VP_INFO("Recreating Bloom processor: {}x{}", m_WindowWidth / 2, m_WindowHeight / 2);
						m_Bloom = std::make_unique<VizEngine::Bloom>(m_WindowWidth / 2, m_WindowHeight / 2);
						m_Bloom->SetThreshold(m_BloomThreshold);
						m_Bloom->SetKnee(m_BloomKnee);
						m_Bloom->SetBlurPasses(m_BloomBlurPasses);
					}
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
				// F2 toggles Framebuffer Preview
				if (event.GetKeyCode() == VizEngine::KeyCode::F2 && !event.IsRepeat())
				{
					m_ShowFramebufferTexture = !m_ShowFramebufferTexture;
					VP_INFO("Framebuffer Preview: {}", m_ShowFramebufferTexture ? "ON" : "OFF");
					return true;  // Consumed
				}
				// F3 toggles Shadow Map Preview
				if (event.GetKeyCode() == VizEngine::KeyCode::F3 && !event.IsRepeat())
				{
					m_ShowShadowMap = !m_ShowShadowMap;
					VP_INFO("Shadow Map Preview: {}", m_ShowShadowMap ? "ON" : "OFF");
					return true;  // Consumed
				}
				// F4 toggles Skybox
				if (event.GetKeyCode() == VizEngine::KeyCode::F4 && !event.IsRepeat())
				{
					m_ShowSkybox = !m_ShowSkybox;
					VP_INFO("Skybox: {}", m_ShowSkybox ? "ON" : "OFF");
					return true;  // Consumed
				}
				return false;
			}
		);
	}

	void OnDestroy() override
	{
		// Clean up raw OpenGL resources not wrapped in RAII
		if (m_ColorGradingLUT != 0)
		{
			VizEngine::Texture::DeleteTexture3D(m_ColorGradingLUT);
			m_ColorGradingLUT = 0;
		}
	}

private:
	// =========================================================================
	// Helper: Compute Light-Space Matrix for Shadow Mapping
	// =========================================================================
	glm::mat4 ComputeLightSpaceMatrix(const VizEngine::DirectionalLight& light)
	{
		// Step 1: Create view matrix looking from light toward scene
		glm::vec3 lightDir = light.GetDirection();  // Normalized direction

		// Position light "behind" the scene (directional lights are infinitely far)
		glm::vec3 lightPos = -lightDir * 15.0f;

		// Handle degenerate up vector (when light direction is vertical)
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		if (glm::abs(glm::dot(lightDir, up)) > 0.999f)
		{
			up = glm::vec3(0.0f, 0.0f, 1.0f);
		}

		glm::mat4 lightView = glm::lookAt(
			lightPos,                      // Light position (behind scene)
			glm::vec3(0.0f, 0.0f, 0.0f),  // Look at origin (scene center)
			up                            // Up vector
		);

		// Step 2: Create orthographic projection
		// Coverage determines how much of the scene gets shadows
		float orthoSize = 15.0f;  // Adjust based on scene size

		glm::mat4 lightProjection = glm::ortho(
			-orthoSize, orthoSize,   // Left, right
			-orthoSize, orthoSize,   // Bottom, top
			0.1f, 30.0f              // Near, far planes
		);

		// Step 3: Combine into light-space matrix
		return lightProjection * lightView;
	}

	// =========================================================================
	// Helper: Render all scene objects with PBR materials
	// =========================================================================
	void RenderSceneObjects()
	{
		auto& renderer = VizEngine::Engine::Get().GetRenderer();

		for (auto& obj : m_Scene)
		{
			if (!obj.Active || !obj.MeshPtr) continue;

			glm::mat4 model = obj.ObjectTransform.GetModelMatrix();
			m_DefaultLitShader->SetMatrix4fv("u_Model", model);

			// Use object's material properties for PBR
			m_DefaultLitShader->SetVec3("u_Albedo", glm::vec3(obj.Color));
			m_DefaultLitShader->SetFloat("u_Metallic", obj.Metallic);
			m_DefaultLitShader->SetFloat("u_Roughness", obj.Roughness);
			m_DefaultLitShader->SetFloat("u_AO", 1.0f);

			// Bind texture if available
			if (obj.TexturePtr)
			{
				obj.TexturePtr->Bind(0);
				m_DefaultLitShader->SetInt("u_AlbedoTexture", 0);
				m_DefaultLitShader->SetBool("u_UseAlbedoTexture", true);
			}
			else
			{
				m_DefaultLitShader->SetBool("u_UseAlbedoTexture", false);
			}

			obj.MeshPtr->Bind();
			renderer.Draw(obj.MeshPtr->GetVertexArray(), obj.MeshPtr->GetIndexBuffer(), *m_DefaultLitShader);
		}
	}

	// Scene
	VizEngine::Scene m_Scene;
	VizEngine::Camera m_Camera;
	VizEngine::DirectionalLight m_Light;

	// Assets
	std::unique_ptr<VizEngine::Shader> m_ShadowDepthShader;
	std::shared_ptr<VizEngine::Texture> m_DefaultTexture;
	std::shared_ptr<VizEngine::Mesh> m_PyramidMesh;
	std::shared_ptr<VizEngine::Mesh> m_CubeMesh;
	std::shared_ptr<VizEngine::Mesh> m_PlaneMesh;

	// Duck model assets (for spawning)
	std::shared_ptr<VizEngine::Mesh> m_DuckMesh;
	std::shared_ptr<VizEngine::Texture> m_DuckTexture;
	glm::vec4 m_DuckColor = glm::vec4(1.0f);
	float m_DuckRoughness = 0.5f;

	// Framebuffer for offscreen rendering
	std::shared_ptr<VizEngine::Framebuffer> m_Framebuffer;
	std::shared_ptr<VizEngine::Texture> m_FramebufferColor;
	std::shared_ptr<VizEngine::Texture> m_FramebufferDepth;
	bool m_ShowFramebufferTexture = true;

	// Shadow mapping
	std::shared_ptr<VizEngine::Framebuffer> m_ShadowMapFramebuffer;
	std::shared_ptr<VizEngine::Texture> m_ShadowMapDepth;
	glm::mat4 m_LightSpaceMatrix;
	bool m_ShowShadowMap = false;

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

	// Skybox
	std::shared_ptr<VizEngine::Texture> m_EnvironmentHDRI;
	std::shared_ptr<VizEngine::Texture> m_SkyboxCubemap;
	std::unique_ptr<VizEngine::Skybox> m_Skybox;
	bool m_ShowSkybox = true;

	// IBL (Chapter 34)
	std::shared_ptr<VizEngine::Texture> m_IrradianceMap;
	std::shared_ptr<VizEngine::Texture> m_PrefilteredMap;
	std::shared_ptr<VizEngine::Texture> m_BRDFLut;
	bool m_UseIBL = true;

	// PBR Rendering (Chapter 33)
	std::unique_ptr<VizEngine::Shader> m_DefaultLitShader;
	std::shared_ptr<VizEngine::Mesh> m_SphereMesh;
	glm::vec3 m_PBRLightPositions[4] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3( 10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3( 10.0f, -10.0f, 10.0f)
	};
	glm::vec3 m_PBRLightColors[4] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	float m_PBRLightIntensity = 300.0f;
	glm::vec3 m_PBRLightColor = glm::vec3(1.0f);  // White light

	// HDR Pipeline (Chapter 35)
	std::shared_ptr<VizEngine::Framebuffer> m_HDRFramebuffer;
	std::shared_ptr<VizEngine::Texture> m_HDRColorTexture;
	std::shared_ptr<VizEngine::Texture> m_HDRDepthTexture;
	std::shared_ptr<VizEngine::Shader> m_ToneMappingShader;
	std::shared_ptr<VizEngine::FullscreenQuad> m_FullscreenQuad;

	// HDR Settings
	int m_ToneMappingMode = 3;      // 0=Reinhard, 1=ReinhardExt, 2=Exposure, 3=ACES, 4=Uncharted2
	float m_Exposure = 1.0f;
	float m_Gamma = 2.2f;
	float m_WhitePoint = 4.0f;      // For Reinhard Extended

	// Post-Processing (Chapter 36)
	std::unique_ptr<VizEngine::Bloom> m_Bloom;
	bool m_EnableBloom = true;
	float m_BloomThreshold = 1.0f;
	float m_BloomKnee = 0.5f;
	float m_BloomIntensity = 0.04f;
	int m_BloomBlurPasses = 5;

	// Color Grading (Chapter 36)
	unsigned int m_ColorGradingLUT = 0;  // Raw OpenGL texture ID
	bool m_EnableColorGrading = false;
	float m_LUTContribution = 1.0f;
	float m_Saturation = 1.0f;
	float m_Contrast = 1.0f;
	float m_Brightness = 0.0f;
};

std::unique_ptr<VizEngine::Application> VizEngine::CreateApplication(VizEngine::EngineConfig& config)
{
	config.Title = "Sandbox - VizPsyche";
	config.Width = 800;
	config.Height = 800;
	return std::make_unique<Sandbox>();
}

