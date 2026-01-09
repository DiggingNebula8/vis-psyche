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
			m_IsValid = false;
			return;
		}

		// ====================================================================
		// Load Shaders
		// ====================================================================
		m_ExtractShader = std::make_shared<Shader>("resources/shaders/bloom_extract.shader");
		m_BlurShader = std::make_shared<Shader>("resources/shaders/bloom_blur.shader");

		// Validate shaders loaded successfully
		if (!m_ExtractShader->IsValid() || !m_BlurShader->IsValid())
		{
			VP_CORE_ERROR("Bloom: Failed to load shaders!");
			m_IsValid = false;
			return;
		}

		// ====================================================================
		// Create Fullscreen Quad
		// ====================================================================
		m_Quad = std::make_shared<FullscreenQuad>();

		// All validations passed - mark as valid
		m_IsValid = true;

		VP_CORE_INFO("Bloom created: {}x{}, {} blur passes", width, height, m_BlurPasses);
	}

	std::shared_ptr<Texture> Bloom::Process(std::shared_ptr<Texture> hdrTexture)
	{
		// Early return if shaders failed to load
		if (!m_IsValid)
		{
			VP_CORE_ERROR("Bloom::Process called on invalid Bloom instance");
			return hdrTexture;  // Return input unchanged
		}

		// Validate input parameter
		if (!hdrTexture)
		{
			VP_CORE_ERROR("Bloom::Process called with null hdrTexture");
			return nullptr;
		}

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

		for (int i = 0; i < m_BlurPasses; ++i)
		{
			// Ping-pong between buffer pairs to avoid read/write conflicts:
			// - If source is Blur1, write horizontal to Blur2, vertical to Blur1
			// - Otherwise (Extract or Blur2), write horizontal to Blur1, vertical to Blur2
			bool sourceIsBlur1 = (sourceTexture == m_BlurTexture1);
			
			std::shared_ptr<Framebuffer> intermediateFB = sourceIsBlur1 ? m_BlurFB2 : m_BlurFB1;
			std::shared_ptr<Texture> intermediateTex = sourceIsBlur1 ? m_BlurTexture2 : m_BlurTexture1;
			std::shared_ptr<Framebuffer> finalFB = sourceIsBlur1 ? m_BlurFB1 : m_BlurFB2;
			std::shared_ptr<Texture> finalTex = sourceIsBlur1 ? m_BlurTexture1 : m_BlurTexture2;

			// Horizontal pass: read from sourceTexture, write to intermediateFB
			intermediateFB->Bind();
			glClear(GL_COLOR_BUFFER_BIT);
			m_BlurShader->SetBool("u_Horizontal", true);
			m_BlurShader->SetInt("u_Image", 0);
			sourceTexture->Bind(0);
			m_Quad->Render();
			intermediateFB->Unbind();

			// Vertical pass: read from intermediateTex, write to finalFB
			finalFB->Bind();
			glClear(GL_COLOR_BUFFER_BIT);
			m_BlurShader->SetBool("u_Horizontal", false);
			m_BlurShader->SetInt("u_Image", 0);
			intermediateTex->Bind(0);
			m_Quad->Render();
			finalFB->Unbind();

			// Update source for next iteration
			sourceTexture = finalTex;
		}
		// Return final blurred result
		return sourceTexture;
	}
}

