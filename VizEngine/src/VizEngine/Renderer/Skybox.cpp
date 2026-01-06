// VizEngine/src/VizEngine/Renderer/Skybox.cpp

#include "Skybox.h"
#include "VizEngine/OpenGL/Texture.h"
#include "VizEngine/OpenGL/Shader.h"
#include "VizEngine/OpenGL/VertexArray.h"
#include "VizEngine/OpenGL/VertexBuffer.h"
#include "VizEngine/Core/Camera.h"
#include "VizEngine/Log.h"

#include <glad/glad.h>
#include <glm.hpp>

namespace VizEngine
{
	Skybox::Skybox(std::shared_ptr<Texture> cubemap)
		: m_Cubemap(cubemap)
	{
		if (!m_Cubemap)
		{
			VP_CORE_ERROR("Skybox: Cubemap texture is null!");
			throw std::runtime_error("Skybox: Cannot create skybox with null cubemap");
		}

		if (!m_Cubemap->IsCubemap())
		{
			VP_CORE_ERROR("Skybox: Texture is not a cubemap!");
			throw std::runtime_error("Skybox: Cannot create skybox from non-cubemap texture");
		}

		// Load skybox shader
		m_Shader = std::make_shared<Shader>("resources/shaders/skybox.shader");

		// Setup cube mesh
		SetupMesh();

		VP_CORE_INFO("Skybox created");
	}

	void Skybox::SetupMesh()
	{
		// Cube vertices (positions only)
		// Skybox cube is centered at origin with size 1
		float skyboxVertices[] = {
			// Positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		// Create VBO
		m_VBO = std::make_unique<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices));

		// Create VAO with layout
		VertexBufferLayout layout;
		layout.Push<float>(3);  // Position only

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->LinkVertexBuffer(*m_VBO, layout);
	}

	void Skybox::Render(const Camera& camera)
	{
		// Disable depth writing (skybox should not block other objects)
		glDepthFunc(GL_LEQUAL);  // Allow depth = 1.0 to pass
		glDepthMask(GL_FALSE);

		m_Shader->Bind();

		// Set uniforms
		m_Shader->SetMatrix4fv("u_View", camera.GetViewMatrix());
		m_Shader->SetMatrix4fv("u_Projection", camera.GetProjectionMatrix());

		// Bind cubemap
		m_Cubemap->Bind(0);
		m_Shader->SetInt("u_Skybox", 0);

		// Render cube
		m_VAO->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Restore depth settings
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
	}
}
