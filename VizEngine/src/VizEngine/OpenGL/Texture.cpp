#include "Texture.h"
#include "VizEngine/Log.h"
#include "stb_image.h"

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
		  m_BPP(other.m_BPP)
	{
		other.m_texture = 0;
		other.m_LocalBuffer = nullptr;
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
			other.m_texture = 0;
			other.m_LocalBuffer = nullptr;
		}
		return *this;
	}

	void Texture::Bind(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::SetFilter(unsigned int minFilter, unsigned int magFilter)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::SetWrap(unsigned int sWrap, unsigned int tWrap)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::SetBorderColor(const float color[4])
	{
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
