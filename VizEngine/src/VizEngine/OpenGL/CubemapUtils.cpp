// VizEngine/src/VizEngine/OpenGL/CubemapUtils.cpp

#include "CubemapUtils.h"
#include "Texture.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VizEngine/Log.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace VizEngine
{
	std::shared_ptr<Texture> CubemapUtils::EquirectangularToCubemap(
		std::shared_ptr<Texture> equirectangularMap,
		int resolution)
	{
		// Validate resolution bounds
		if (resolution <= 0 || resolution > 8192)
		{
			VP_CORE_ERROR("Cubemap conversion: Invalid resolution {} (must be 1-8192)", resolution);
			return nullptr;
		}

		if (!equirectangularMap)
		{
			VP_CORE_ERROR("Cubemap conversion: Null equirectangularMap passed to EquirectangularToCubemap");
			return nullptr;
		}

		VP_CORE_INFO("Converting equirectangular map to cubemap ({}x{} per face)...", resolution, resolution);

		// Create empty cubemap texture
		auto cubemap = std::make_shared<Texture>(resolution, equirectangularMap->IsHDR());

		// Create framebuffer for rendering to cubemap faces
		auto framebuffer = std::make_shared<Framebuffer>(resolution, resolution);

		// Create depth renderbuffer (we don't need to sample it)
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Attach depth renderbuffer to framebuffer
		framebuffer->Bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		// Load conversion shader
		auto shader = std::make_shared<Shader>("resources/shaders/equirect_to_cube.shader");

		// Projection matrix (90° FOV to cover each face exactly)
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		// View matrices for each cubemap face
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),  // +X
			glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),  // -X
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),  // +Y
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),  // -Y
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),  // +Z
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))   // -Z
		};

		// Cube vertices (positions only, for fullscreen rendering)
		float cubeVertices[] = {
			// Positions
			-1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,

			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,

			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f
		};

		// Setup cube VAO/VBO
		auto cubeVBO = std::make_shared<VertexBuffer>(cubeVertices, sizeof(cubeVertices));
		VertexBufferLayout layout;
		layout.Push<float>(3);  // Position

		auto cubeVAO = std::make_shared<VertexArray>();
		cubeVAO->LinkVertexBuffer(*cubeVBO, layout);

		// Bind shader and equirectangular map
		shader->Bind();
		shader->SetMatrix4fv("u_Projection", captureProjection);
		equirectangularMap->Bind(0);
		shader->SetInt("u_EquirectangularMap", 0);

		// Save current viewport
		GLint prevViewport[4];
		glGetIntegerv(GL_VIEWPORT, prevViewport);

		glViewport(0, 0, resolution, resolution);
		framebuffer->Bind();

		// Render to each cubemap face
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader->SetMatrix4fv("u_View", captureViews[i]);

			// Attach current cubemap face to framebuffer
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				cubemap->GetID(),
				0  // Mipmap level
			);

			// Verify framebuffer is complete after attachment
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				VP_CORE_ERROR("Cubemap conversion: FBO incomplete for face {}", i);
				glDeleteRenderbuffers(1, &rbo);
				return nullptr;
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Render cube
			cubeVAO->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		framebuffer->Unbind();

		// Restore previous viewport
		glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

		// Generate mipmaps for the cubemap (improves quality and required for IBL)
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->GetID());
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		// Cleanup
		glDeleteRenderbuffers(1, &rbo);

		VP_CORE_INFO("Cubemap conversion complete (with mipmaps)!");

		return cubemap;
	}
}
