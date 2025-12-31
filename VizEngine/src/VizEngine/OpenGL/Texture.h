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
		
		~Texture();

		// Prevent copying (Rule of 5)
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// Allow moving
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline unsigned int GetID() const { return m_RendererID; }

	private:
		unsigned int m_RendererID;
		std::string m_FilePath;
		unsigned char* m_LocalBuffer;
		int m_Width, m_Height, m_BPP;
	};
}