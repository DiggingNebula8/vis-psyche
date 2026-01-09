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

	/**
	 * Load HDR equirectangular image (for environment maps).
	 * Uses stbi_loadf for floating-point data.
	 * @param filepath Path to .hdr file
	 * @param isHDR Set to true to load as HDR (GL_RGB16F)
	 */
	Texture(const std::string& filepath, bool isHDR);

	/**
	 * Create an empty cubemap texture.
	 * @param resolution Resolution per face (e.g., 512, 1024)
	 * @param isHDR Use HDR format (GL_RGB16F) or LDR (GL_RGB8)
	 */
	Texture(int resolution, bool isHDR);
		
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
		inline bool IsCubemap() const { return m_IsCubemap; }
	inline bool IsHDR() const { return m_IsHDR; }

	// =========================================================================
	// Static Utility Methods
	// =========================================================================

	/**
	 * Create a neutral (identity) 3D color grading LUT.
	 * @param size LUT dimensions (e.g., 16 for 16x16x16)
	 * @return OpenGL texture ID for GL_TEXTURE_3D (caller owns, must delete)
	 */
	static unsigned int CreateNeutralLUT3D(int size = 16);

	/**
	 * Bind a 3D texture (e.g., color grading LUT) to a texture unit.
	 * @param textureID OpenGL texture ID for GL_TEXTURE_3D
	 * @param slot Texture unit (0-15)
	 */
	static void BindTexture3D(unsigned int textureID, unsigned int slot);

	/**
	 * Delete a 3D texture created by CreateNeutralLUT3D.
	 * @param textureID OpenGL texture ID to delete
	 */
	static void DeleteTexture3D(unsigned int textureID);

	private:
		unsigned int m_texture;
		std::string m_FilePath;
		unsigned char* m_LocalBuffer;
		int m_Width, m_Height, m_BPP;
		bool m_IsCubemap = false;
		bool m_IsHDR = false;
	};
}