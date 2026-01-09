#include "Texture.h"
#include "VizEngine/Log.h"
#include "stb_image.h"
#include <vector>

namespace VizEngine
{
	Texture::Texture(const std::string& path)
		: m_texture(0), m_FilePath(path), m_LocalBuffer(nullptr),
		  m_Width(0), m_Height(0), m_BPP(0)
	{
		stbi_set_flip_vertically_on_load(1);
		m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

		if (!m_LocalBuffer)
		{
			VP_CORE_ERROR("Failed to load texture: {}", path);
			return;
		}

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(m_LocalBuffer);
		m_LocalBuffer = nullptr;
	}

	Texture::Texture(const unsigned char* data, int width, int height, int channels)
		: m_texture(0), m_FilePath("embedded"), m_LocalBuffer(nullptr),
		  m_Width(width), m_Height(height), m_BPP(channels)
	{
		if (!data || width <= 0 || height <= 0 || channels <= 0)
		{
			VP_CORE_ERROR("Failed to create texture from raw data: invalid parameters (data, width, height, or channels)");
			return;
		}

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Determine format based on channels
		GLenum internalFormat = GL_RGBA8;
		GLenum dataFormat = GL_RGBA;
		
		if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 2)
		{
			internalFormat = GL_RG8;
			dataFormat = GL_RG;
		}
		else if (channels == 1)
		{
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}
		else if (channels != 4)
		{
			VP_CORE_WARN("Unsupported channel count: {}, defaulting to RGBA", channels);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(int width, int height, unsigned int internalFormat, unsigned int format, unsigned int dataType)
		: m_texture(0), m_FilePath("framebuffer"), m_LocalBuffer(nullptr),
		  m_Width(width), m_Height(height), m_BPP(4)
	{
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		// Allocate texture storage (data = nullptr for empty texture)
		glTexImage2D(
			GL_TEXTURE_2D,
			0,                    // mipmap level
			internalFormat,       // e.g., GL_RGBA8 or GL_DEPTH_COMPONENT24
			m_Width,
			m_Height,
			0,                    // border (must be 0)
			format,               // e.g., GL_RGBA or GL_DEPTH_COMPONENT
			dataType,             // e.g., GL_UNSIGNED_BYTE or GL_FLOAT
			nullptr               // no pixel data (allocate empty)
		);

		// Set texture parameters suitable for framebuffer attachments
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		VP_CORE_INFO("Empty texture created: ID={}, Size={}x{}", m_texture, m_Width, m_Height);
	}

	Texture::Texture(const std::string& filepath, bool isHDR)
		: m_texture(0), m_FilePath(filepath), m_LocalBuffer(nullptr),
		  m_Width(0), m_Height(0), m_BPP(0), m_IsHDR(isHDR)
	{
		// stb_image loads with bottom-left origin, OpenGL expects bottom-left
		stbi_set_flip_vertically_on_load(1);

		if (m_IsHDR)
		{
			// Load HDR image (floating-point data)
			float* hdrData = stbi_loadf(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 0);

			if (hdrData)
			{
				glGenTextures(1, &m_texture);
				glBindTexture(GL_TEXTURE_2D, m_texture);

				// Upload as 16-bit float texture (GL_RGB16F)
				glTexImage2D(
					GL_TEXTURE_2D,
					0,                  // Mipmap level
					GL_RGB16F,          // Internal format (HDR)
					m_Width,
					m_Height,
					0,                  // Border
					GL_RGB,             // Format
					GL_FLOAT,           // Data type
					hdrData
				);

				// Set texture parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				VP_CORE_INFO("HDR Texture loaded: {} ({}x{}, {} channels)", filepath, m_Width, m_Height, m_BPP);

				stbi_image_free(hdrData);
			}
			else
			{
				VP_CORE_ERROR("Failed to load HDR texture: {}", filepath);
			}
		}
		else
		{
			// Regular LDR loading (delegate to existing logic)
			unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

			if (data)
			{
				glGenTextures(1, &m_texture);
				glBindTexture(GL_TEXTURE_2D, m_texture);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				VP_CORE_INFO("LDR Texture loaded: {} ({}x{}, {} channels)", filepath, m_Width, m_Height, m_BPP);

				stbi_image_free(data);
			}
			else
			{
				VP_CORE_ERROR("Failed to load LDR texture: {}", filepath);
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(int resolution, bool isHDR)
		: m_texture(0), m_FilePath("cubemap"), m_LocalBuffer(nullptr),
		  m_Width(resolution), m_Height(resolution), m_BPP(3),
		  m_IsCubemap(true), m_IsHDR(isHDR)
	{
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

		// Allocate storage for all 6 faces
		GLenum internalFormat = isHDR ? GL_RGB16F : GL_RGB8;
		GLenum format = GL_RGB;
		GLenum type = isHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,  // Face target
				0,                                    // Mipmap level
				internalFormat,
				m_Width,
				m_Height,
				0,                                    // Border
				format,
				type,
				nullptr                               // No data (allocate empty)
			);
		}

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		VP_CORE_INFO("Empty cubemap created: {}x{} per face ({})", 
			m_Width, m_Height, isHDR ? "HDR" : "LDR");
	}

	Texture::~Texture()
	{
		if (m_texture != 0)
		{
			glDeleteTextures(1, &m_texture);
		}
	}

	// Move constructor
	Texture::Texture(Texture&& other) noexcept
		: m_texture(other.m_texture),
		  m_FilePath(std::move(other.m_FilePath)),
		  m_LocalBuffer(other.m_LocalBuffer),
		  m_Width(other.m_Width),
		  m_Height(other.m_Height),
		  m_BPP(other.m_BPP),
		  m_IsCubemap(other.m_IsCubemap),
		  m_IsHDR(other.m_IsHDR)
	{
		other.m_texture = 0;
		other.m_LocalBuffer = nullptr;
		other.m_IsCubemap = false;
		other.m_IsHDR = false;
	}

	// Move assignment operator
	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_texture != 0)
			{
				glDeleteTextures(1, &m_texture);
			}
			m_texture = other.m_texture;
			m_FilePath = std::move(other.m_FilePath);
			m_LocalBuffer = other.m_LocalBuffer;
			m_Width = other.m_Width;
			m_Height = other.m_Height;
			m_BPP = other.m_BPP;
			m_IsCubemap = other.m_IsCubemap;
			m_IsHDR = other.m_IsHDR;
			other.m_texture = 0;
			other.m_LocalBuffer = nullptr;
			other.m_IsCubemap = false;
			other.m_IsHDR = false;
		}
		return *this;
	}

	void Texture::Bind(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		
		if (m_IsCubemap)
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
		else
			glBindTexture(GL_TEXTURE_2D, m_texture);
	}

	void Texture::Unbind() const
	{
		if (m_IsCubemap)
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		else
			glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::SetFilter(unsigned int minFilter, unsigned int magFilter)
	{
		GLenum target = m_IsCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
		glBindTexture(target, m_texture);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
		glBindTexture(target, 0);
	}

	void Texture::SetWrap(unsigned int sWrap, unsigned int tWrap)
	{
		GLenum target = m_IsCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
		glBindTexture(target, m_texture);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, tWrap);
		glBindTexture(target, 0);
	}

	void Texture::SetBorderColor(const float color[4])
	{
		GLenum target = m_IsCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
		glBindTexture(target, m_texture);
		glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color);
		glBindTexture(target, 0);
	}

	// =========================================================================
	// Static Utility Methods
	// =========================================================================

	unsigned int Texture::CreateNeutralLUT3D(int size)
	{
		// Validate size parameter to prevent division by zero and invalid textures
		if (size <= 0)
		{
			VP_CORE_ERROR("CreateNeutralLUT3D: Invalid size {} (must be > 0)", size);
			return 0;
		}

		if (size == 1)
		{
			VP_CORE_WARN("CreateNeutralLUT3D: size=1 would cause division by zero, clamping to 2");
			size = 2;
		}

		const int totalTexels = size * size * size;
		std::vector<float> lutData(totalTexels * 3);

		// Generate identity mapping: input RGB = output RGB
		for (int b = 0; b < size; ++b)
		{
			for (int g = 0; g < size; ++g)
			{
				for (int r = 0; r < size; ++r)
				{
					int index = (b * size * size + g * size + r) * 3;
					lutData[index + 0] = r / float(size - 1);
					lutData[index + 1] = g / float(size - 1);
					lutData[index + 2] = b / float(size - 1);
				}
			}
		}

		// Create 3D texture
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_3D, textureID);

		glTexImage3D(
			GL_TEXTURE_3D,
			0,                      // Mip level
			GL_RGB16F,              // Internal format (HDR precision)
			size, size, size,
			0,                      // Border
			GL_RGB,                 // Format
			GL_FLOAT,               // Data type
			lutData.data()
		);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_3D, 0);

		VP_CORE_INFO("Neutral 3D LUT created: {}x{}x{} (ID: {})", size, size, size, textureID);

		return textureID;
	}

	void Texture::BindTexture3D(unsigned int textureID, unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_3D, textureID);
	}

	void Texture::DeleteTexture3D(unsigned int textureID)
	{
		if (textureID != 0)
		{
			glDeleteTextures(1, &textureID);
		}
	}
}
