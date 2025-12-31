#include "Texture.h"
#include "stb_image.h"

namespace VizEngine
{
	Texture::Texture(const std::string& path)
		: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
		  m_Width(0), m_Height(0), m_BPP(0)
	{
		stbi_set_flip_vertically_on_load(1);
		m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

		if (!m_LocalBuffer)
		{
			std::cerr << "Failed to load texture: " << path << std::endl;
			return;
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(m_LocalBuffer);
		m_LocalBuffer = nullptr;
	}

	Texture::Texture(const unsigned char* data, int width, int height, int channels)
		: m_RendererID(0), m_FilePath("embedded"), m_LocalBuffer(nullptr),
		  m_Width(width), m_Height(height), m_BPP(channels)
	{
		if (!data || width <= 0 || height <= 0)
		{
			std::cerr << "Failed to create texture from raw data: invalid parameters" << std::endl;
			return;
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
		else if (channels == 1)
		{
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::~Texture()
	{
		if (m_RendererID != 0)
		{
			glDeleteTextures(1, &m_RendererID);
		}
	}

	// Move constructor
	Texture::Texture(Texture&& other) noexcept
		: m_RendererID(other.m_RendererID),
		  m_FilePath(std::move(other.m_FilePath)),
		  m_LocalBuffer(other.m_LocalBuffer),
		  m_Width(other.m_Width),
		  m_Height(other.m_Height),
		  m_BPP(other.m_BPP)
	{
		other.m_RendererID = 0;
		other.m_LocalBuffer = nullptr;
	}

	// Move assignment operator
	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_RendererID != 0)
			{
				glDeleteTextures(1, &m_RendererID);
			}
			m_RendererID = other.m_RendererID;
			m_FilePath = std::move(other.m_FilePath);
			m_LocalBuffer = other.m_LocalBuffer;
			m_Width = other.m_Width;
			m_Height = other.m_Height;
			m_BPP = other.m_BPP;
			other.m_RendererID = 0;
			other.m_LocalBuffer = nullptr;
		}
		return *this;
	}

	void Texture::Bind(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
