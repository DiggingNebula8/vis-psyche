// VizEngine/src/VizEngine/Renderer/Bloom.h

#pragma once

#include "VizEngine/Core.h"
#include <memory>

namespace VizEngine
{
	class Framebuffer;
	class Shader;
	class Texture;
	class FullscreenQuad;

	/**
	 * Bloom post-processing effect.
	 * Extracts bright regions, applies Gaussian blur, returns bloom texture.
	 */
	class VizEngine_API Bloom
	{
	public:
		/**
		 * Create bloom processor.
		 * @param width Bloom buffer width (typically half of scene resolution)
		 * @param height Bloom buffer height
		 */
		Bloom(int width, int height);
		~Bloom() = default;

		/**
		 * Process HDR texture to generate bloom.
		 * @param hdrTexture Input HDR framebuffer color attachment
		 * @return Bloom texture (same resolution as bloom buffers)
		 */
		std::shared_ptr<Texture> Process(std::shared_ptr<Texture> hdrTexture);

		// Settings
		void SetThreshold(float threshold) { m_Threshold = threshold; }
		void SetKnee(float knee) { m_Knee = knee; }
		void SetIntensity(float intensity) { m_Intensity = intensity; }
		void SetBlurPasses(int passes) { m_BlurPasses = passes; }

		float GetThreshold() const { return m_Threshold; }
		float GetKnee() const { return m_Knee; }
		float GetIntensity() const { return m_Intensity; }
		int GetBlurPasses() const { return m_BlurPasses; }

	private:
		// Framebuffers for multi-pass rendering
		std::shared_ptr<Framebuffer> m_ExtractFB;
		std::shared_ptr<Framebuffer> m_BlurFB1;  // Ping-pong buffer 1
		std::shared_ptr<Framebuffer> m_BlurFB2;  // Ping-pong buffer 2

		// Textures (attached to framebuffers)
		std::shared_ptr<Texture> m_ExtractTexture;
		std::shared_ptr<Texture> m_BlurTexture1;
		std::shared_ptr<Texture> m_BlurTexture2;

		// Shaders
		std::shared_ptr<Shader> m_ExtractShader;
		std::shared_ptr<Shader> m_BlurShader;

		// Fullscreen quad for rendering
		std::shared_ptr<FullscreenQuad> m_Quad;

		// Bloom parameters
		float m_Threshold = 1.0f;    // Brightness threshold for bloom
		float m_Knee = 0.1f;         // Soft threshold range
		float m_Intensity = 0.04f;   // Bloom intensity (set via composite, but stored here)
		int m_BlurPasses = 5;        // Number of blur iterations (more = softer)

		int m_Width, m_Height;
	};
}
