#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>
#include "VizEngine/Core.h"

namespace VizEngine
{
	class VizEngine_API Texture
	{
	public:
		// Load from file
		Texture(const std::string& path);
		
		// Create from raw pixel data (for embedded textures in glTF)
		Texture(const unsigned char* data, int width, int height, int channels = 4);
	
	/**
	 * Create an empty texture for use as a framebuffer attachment.
	 * @param width Texture width
	 * @param height Texture height
	 * @param internalFormat OpenGL internal format (e.g., GL_RGBA8, GL_DEPTH_COMPONENT24)
	 * @param format OpenGL format (e.g., GL_RGBA, GL_DEPTH_COMPONENT)
	 * @param dataType Data type (e.g., GL_UNSIGNED_BYTE, GL_FLOAT)
	 */
	Texture(int width, int height, unsigned int internalFormat, unsigned int format, unsigned int dataType);
		
		~Texture();

		// Prevent copying (Rule of 5)
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// Allow moving
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		// Configuration
		void SetFilter(unsigned int minFilter, unsigned int magFilter);
		void SetWrap(unsigned int sWrap, unsigned int tWrap);
		void SetBorderColor(const float color[4]);

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline unsigned int GetID() const { return m_texture; }

	private:
		unsigned int m_texture;
		std::string m_FilePath;
		unsigned char* m_LocalBuffer;
		int m_Width, m_Height, m_BPP;
	};
}