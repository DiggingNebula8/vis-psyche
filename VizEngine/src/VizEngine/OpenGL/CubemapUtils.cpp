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
#include <cmath>

namespace VizEngine
{
	std::shared_ptr<Texture> CubemapUtils::EquirectangularToCubemap(
		std::shared_ptr<Texture> equirectangularMap,
		int resolution)
	{
		if (!equirectangularMap)
		{
			VP_CORE_ERROR("Cubemap conversion: Input texture is null!");
			return nullptr;
		}

		// Validate resolution bounds
		if (resolution <= 0 || resolution > 8192)
		{
			VP_CORE_ERROR("Cubemap conversion: Invalid resolution {} (must be 1-8192)", resolution);
			return nullptr;
		}

		VP_CORE_INFO("Converting equirectangular map to cubemap ({}x{} per face)...", resolution, resolution);

		// Create empty cubemap texture
		auto cubemap = std::make_shared<Texture>(resolution, equirectangularMap->IsHDR());
		if (!cubemap)
		{
			VP_CORE_ERROR("Cubemap conversion: Failed to create cubemap texture (resolution: {}x{})", resolution, resolution);
			return nullptr;
		}

		// Create framebuffer for rendering to cubemap faces
		auto framebuffer = std::make_shared<Framebuffer>(resolution, resolution);

		if (!framebuffer)
		{
			VP_CORE_ERROR("Cubemap conversion: Failed to create framebuffer");
			return nullptr;
		}

		// Create depth renderbuffer (we don't need to sample it)
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		if (rbo == 0)
		{
			VP_CORE_ERROR("Cubemap conversion: Failed to generate renderbuffer");
			return nullptr;
		}

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Attach depth renderbuffer to framebuffer
		framebuffer->Bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		// Verify framebuffer usability (at least depth attached)
		if (!framebuffer->IsComplete())
		{
			VP_CORE_ERROR("Cubemap conversion: Framebuffer incomplete after depth attachment");
			glDeleteRenderbuffers(1, &rbo);
			return nullptr;
		}

		// Load conversion shader
		auto shader = std::make_shared<Shader>("resources/shaders/equirect_to_cube.shader");
		if (!shader->IsValid())
		{
			VP_CORE_ERROR("Cubemap conversion: Failed to load shader 'resources/shaders/equirect_to_cube.shader'");
			glDeleteRenderbuffers(1, &rbo);
			return nullptr;
		}

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
		auto cubeVBO = std::make_shared<VertexBuffer>(cubeVertices, static_cast<unsigned int>(sizeof(cubeVertices)));
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
				glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
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

	std::shared_ptr<Texture> CubemapUtils::GenerateIrradianceMap(
		std::shared_ptr<Texture> environmentMap,
		int resolution)
	{
		if (!environmentMap || !environmentMap->IsCubemap())
		{
			VP_CORE_ERROR("GenerateIrradianceMap: Input must be a cubemap!");
			return nullptr;
		}

		VP_CORE_INFO("Generating irradiance map ({}x{})...", resolution, resolution);

		// Create empty irradiance cubemap
		auto irradianceMap = std::make_shared<Texture>(resolution, true);

		// Create framebuffer
		auto framebuffer = std::make_shared<Framebuffer>(resolution, resolution);
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);

		framebuffer->Bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		// Verify framebuffer completeness before proceeding
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			VP_CORE_ERROR("GenerateIrradianceMap: Framebuffer incomplete after depth attachment");
			glDeleteRenderbuffers(1, &rbo);
			framebuffer->Unbind();
			return nullptr;
		}

		// Load irradiance convolution shader
		auto shader = std::make_shared<Shader>("resources/shaders/irradiance_convolution.shader");
		if (!shader->IsValid())
		{
			VP_CORE_ERROR("Failed to load irradiance_convolution.shader");
			glDeleteRenderbuffers(1, &rbo);
			framebuffer->Unbind();
			return nullptr;
		}

		// Capture matrices (same as equirect conversion)
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		// Cube vertices (same as EquirectangularToCubemap)
		float cubeVertices[] = {
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

		auto cubeVBO = std::make_shared<VertexBuffer>(cubeVertices, static_cast<unsigned int>(sizeof(cubeVertices)));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		auto cubeVAO = std::make_shared<VertexArray>();
		cubeVAO->LinkVertexBuffer(*cubeVBO, layout);

		shader->Bind();
		shader->SetMatrix4fv("u_Projection", captureProjection);
		environmentMap->Bind(0);
		shader->SetInt("u_EnvironmentMap", 0);

		GLint prevViewport[4];
		glGetIntegerv(GL_VIEWPORT, prevViewport);
		glViewport(0, 0, resolution, resolution);

		for (unsigned int i = 0; i < 6; ++i)
		{
			shader->SetMatrix4fv("u_View", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap->GetID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cubeVAO->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		framebuffer->Unbind();
		glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
		glDeleteRenderbuffers(1, &rbo);

		VP_CORE_INFO("Irradiance map complete!");
		return irradianceMap;
	}

	std::shared_ptr<Texture> CubemapUtils::GeneratePrefilteredMap(
		std::shared_ptr<Texture> environmentMap,
		int resolution)
	{
		if (!environmentMap || !environmentMap->IsCubemap())
		{
			VP_CORE_ERROR("GeneratePrefilteredMap: Input must be a cubemap!");
			return nullptr;
		}

		VP_CORE_INFO("Generating pre-filtered environment map ({}x{})...", resolution, resolution);

		// Create empty cubemap with mipmaps
		auto prefilteredMap = std::make_shared<Texture>(resolution, true);

		// Enable mipmaps and allocate mip storage
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMap->GetID());
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		// Load prefilter shader
		auto shader = std::make_shared<Shader>("resources/shaders/prefilter_environment.shader");
		if (!shader->IsValid())
		{
			VP_CORE_ERROR("Failed to load prefilter_environment.shader");
			return nullptr;
		}

		// Capture matrices
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		// Cube geometry (same as other methods)
		float cubeVertices[] = {
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

		auto cubeVBO = std::make_shared<VertexBuffer>(cubeVertices, static_cast<unsigned int>(sizeof(cubeVertices)));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		auto cubeVAO = std::make_shared<VertexArray>();
		cubeVAO->LinkVertexBuffer(*cubeVBO, layout);

		shader->Bind();
		shader->SetMatrix4fv("u_Projection", captureProjection);
		environmentMap->Bind(0);
		shader->SetInt("u_EnvironmentMap", 0);

		// Create framebuffer
		auto framebuffer = std::make_shared<Framebuffer>(resolution, resolution);
		framebuffer->Bind();

		// Verify framebuffer is usable before entering mip loop
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			VP_CORE_ERROR("GeneratePrefilteredMap: Framebuffer not usable");
			framebuffer->Unbind();
			return nullptr;
		}

		GLint prevViewport[4];
		glGetIntegerv(GL_VIEWPORT, prevViewport);

		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// Calculate mip dimensions
			unsigned int mipWidth = static_cast<unsigned int>(resolution * std::pow(0.5f, mip));
			unsigned int mipHeight = mipWidth;

			// Create depth buffer for this mip size
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

			glViewport(0, 0, mipWidth, mipHeight);

			// Roughness for this mip level
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			shader->SetFloat("u_Roughness", roughness);

			// Render to each face at this mip level
			for (unsigned int i = 0; i < 6; ++i)
			{
				shader->SetMatrix4fv("u_View", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredMap->GetID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				cubeVAO->Bind();
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glDeleteRenderbuffers(1, &rbo);
		}

		framebuffer->Unbind();
		glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

		VP_CORE_INFO("Pre-filtered environment map complete ({} mip levels)!", maxMipLevels);
		return prefilteredMap;
	}

	std::shared_ptr<Texture> CubemapUtils::GenerateBRDFLUT(int resolution)
	{
		VP_CORE_INFO("Generating BRDF LUT ({}x{})...", resolution, resolution);

		// Create 2D texture (RG16F format for scale + bias)
		auto brdfLUT = std::make_shared<Texture>(
			resolution, resolution,
			GL_RG16F, GL_RG, GL_FLOAT
		);

		// Load BRDF integration shader
		auto shader = std::make_shared<Shader>("resources/shaders/brdf_integration.shader");
		if (!shader->IsValid())
		{
			VP_CORE_ERROR("Failed to load brdf_integration.shader");
			return nullptr;
		}

		// Create framebuffer
		auto framebuffer = std::make_shared<Framebuffer>(resolution, resolution);
		framebuffer->AttachColorTexture(brdfLUT, 0);

		if (!framebuffer->IsComplete())
		{
			VP_CORE_ERROR("BRDF LUT framebuffer incomplete!");
			return nullptr;
		}

		// Fullscreen quad (clip space, position + UV)
		float quadVertices[] = {
			// Positions    // TexCoords
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		};

		auto quadVBO = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));
		VertexBufferLayout layout;
		layout.Push<float>(3);  // Position
		layout.Push<float>(2);  // TexCoords

		auto quadVAO = std::make_shared<VertexArray>();
		quadVAO->LinkVertexBuffer(*quadVBO, layout);

		GLint prevViewport[4];
		glGetIntegerv(GL_VIEWPORT, prevViewport);

		framebuffer->Bind();
		glViewport(0, 0, resolution, resolution);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->Bind();
		quadVAO->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		framebuffer->Unbind();
		glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

		VP_CORE_INFO("BRDF LUT complete!");
		return brdfLUT;
	}
}

