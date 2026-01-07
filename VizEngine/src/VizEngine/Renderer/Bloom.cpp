// VizEngine/src/VizEngine/Renderer/Bloom.cpp

#include "Bloom.h"
#include "VizEngine/OpenGL/Framebuffer.h"
#include "VizEngine/OpenGL/Shader.h"
#include "VizEngine/OpenGL/Texture.h"
#include "VizEngine/OpenGL/FullscreenQuad.h"
#include "VizEngine/Log.h"

#include <glad/glad.h>

namespace VizEngine
{
	Bloom::Bloom(int width, int height)
		: m_Width(width), m_Height(height)
	{
		// ====================================================================
		// Create Textures (RGB16F for HDR bloom)
		// ====================================================================
		m_ExtractTexture = std::make_shared<Texture>(
			width, height, GL_RGB16F, GL_RGB, GL_FLOAT
		);
		m_BlurTexture1 = std::make_shared<Texture>(
			width, height, GL_RGB16F, GL_RGB, GL_FLOAT
		);
		m_BlurTexture2 = std::make_shared<Texture>(
			width, height, GL_RGB16F, GL_RGB, GL_FLOAT
		);

		// ====================================================================
		// Create Framebuffers
		// ====================================================================
		m_ExtractFB = std::make_shared<Framebuffer>(width, height);
		m_ExtractFB->AttachColorTexture(m_ExtractTexture, 0);
		
		m_BlurFB1 = std::make_shared<Framebuffer>(width, height);
		m_BlurFB1->AttachColorTexture(m_BlurTexture1, 0);
		
		m_BlurFB2 = std::make_shared<Framebuffer>(width, height);
		m_BlurFB2->AttachColorTexture(m_BlurTexture2, 0);

		// Verify framebuffers are complete
		if (!m_ExtractFB->IsComplete() || !m_BlurFB1->IsComplete() || !m_BlurFB2->IsComplete())
		{
			VP_CORE_ERROR("Bloom: Framebuffers not complete!");
		}

		// ====================================================================
		// Load Shaders
		// ====================================================================
		m_ExtractShader = std::make_shared<Shader>("resources/shaders/bloom_extract.shader");
		m_BlurShader = std::make_shared<Shader>("resources/shaders/bloom_blur.shader");

		// ====================================================================
		// Create Fullscreen Quad
		// ====================================================================
		m_Quad = std::make_shared<FullscreenQuad>();

		VP_CORE_INFO("Bloom created: {}x{}, {} blur passes", width, height, m_BlurPasses);
	}

	std::shared_ptr<Texture> Bloom::Process(std::shared_ptr<Texture> hdrTexture)
	{
		// ====================================================================
		// Pass 1: Extract Bright Regions
		// ====================================================================
		m_ExtractFB->Bind();
		glClear(GL_COLOR_BUFFER_BIT);

		m_ExtractShader->Bind();
		m_ExtractShader->SetInt("u_HDRBuffer", 0);
		m_ExtractShader->SetFloat("u_Threshold", m_Threshold);
		m_ExtractShader->SetFloat("u_Knee", m_Knee);

		hdrTexture->Bind(0);
		m_Quad->Render();

		m_ExtractFB->Unbind();

		// ====================================================================
		// Pass 2: Blur (Ping-Pong between two framebuffers)
		// ====================================================================
		m_BlurShader->Bind();
		m_BlurShader->SetVec2("u_TextureSize", glm::vec2(m_Width, m_Height));

		std::shared_ptr<Texture> sourceTexture = m_ExtractTexture;
		std::shared_ptr<Framebuffer> targetFB;

		for (int i = 0; i < m_BlurPasses; ++i)
		{
			// Horizontal pass
			targetFB = (i % 2 == 0) ? m_BlurFB1 : m_BlurFB2;
			targetFB->Bind();
			glClear(GL_COLOR_BUFFER_BIT);

			m_BlurShader->SetBool("u_Horizontal", true);
			m_BlurShader->SetInt("u_Image", 0);

			sourceTexture->Bind(0);
			m_Quad->Render();

			targetFB->Unbind();

			// Update source for next pass
			sourceTexture = (i % 2 == 0) ? m_BlurTexture1 : m_BlurTexture2;

			// Vertical pass
			targetFB = (i % 2 == 0) ? m_BlurFB2 : m_BlurFB1;
			targetFB->Bind();
			glClear(GL_COLOR_BUFFER_BIT);

			m_BlurShader->SetBool("u_Horizontal", false);
			m_BlurShader->SetInt("u_Image", 0);

			sourceTexture->Bind(0);
			m_Quad->Render();

			targetFB->Unbind();

			// Update source for next iteration
			sourceTexture = (i % 2 == 0) ? m_BlurTexture2 : m_BlurTexture1;
		}

		// Return final blurred result
		return sourceTexture;
	}
}
