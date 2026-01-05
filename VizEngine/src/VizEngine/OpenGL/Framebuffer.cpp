// VizEngine/src/VizEngine/OpenGL/Framebuffer.cpp

#include "Framebuffer.h"
#include "Texture.h"
#include "VizEngine/Log.h"

#include <glad/glad.h>

namespace VizEngine
{
	Framebuffer::Framebuffer(int width, int height)
		: m_Width(width), m_Height(height)
	{
		// Generate framebuffer object
		glGenFramebuffers(1, &m_FBO);
		VP_CORE_INFO("Framebuffer created: ID={}, Size={}x{}", m_FBO, m_Width, m_Height);
	}

	Framebuffer::~Framebuffer()
	{
		if (m_FBO != 0)
		{
			VP_CORE_INFO("Framebuffer destroyed: ID={}", m_FBO);
			glDeleteFramebuffers(1, &m_FBO);
			m_FBO = 0;
		}
	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		// Set viewport to match framebuffer size
		glViewport(0, 0, m_Width, m_Height);
	}

	void Framebuffer::Unbind() const
	{
		// Bind default framebuffer (the screen)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::AttachColorTexture(std::shared_ptr<Texture> texture, int slot)
	{
		if (slot < 0 || slot >= 8)
		{
			VP_CORE_ERROR("Framebuffer: Color attachment slot {} out of range [0-7]", slot);
			return;
		}

		if (!texture)
		{
			VP_CORE_ERROR("Framebuffer: Cannot attach null texture to color slot {}", slot);
			return;
		}

		// Verify texture dimensions match framebuffer
		if (texture->GetWidth() != m_Width || texture->GetHeight() != m_Height)
		{
			VP_CORE_WARN("Framebuffer: Texture dimensions ({}x{}) don't match framebuffer ({}x{})",
				texture->GetWidth(), texture->GetHeight(), m_Width, m_Height);
		}

		// Bind framebuffer and attach texture
		Bind();
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + slot,
			GL_TEXTURE_2D,
			texture->GetID(),
			0  // mipmap level
		);

		// Store reference to keep texture alive
		m_ColorAttachments[slot] = texture;

		VP_CORE_INFO("Framebuffer {}: Attached color texture {} to slot {}", m_FBO, texture->GetID(), slot);
	}

	void Framebuffer::AttachDepthTexture(std::shared_ptr<Texture> texture)
	{
		if (!texture)
		{
			VP_CORE_ERROR("Framebuffer: Cannot attach null depth texture");
			return;
		}

		// Verify texture dimensions match framebuffer
		if (texture->GetWidth() != m_Width || texture->GetHeight() != m_Height)
		{
			VP_CORE_WARN("Framebuffer: Depth texture dimensions ({}x{}) don't match framebuffer ({}x{})",
				texture->GetWidth(), texture->GetHeight(), m_Width, m_Height);
		}

		// Bind framebuffer and attach depth texture
		Bind();
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D,
			texture->GetID(),
			0  // mipmap level
		);

		// Store reference
		m_DepthAttachment = texture;

		VP_CORE_INFO("Framebuffer {}: Attached depth texture {}", m_FBO, texture->GetID());
	}

	bool Framebuffer::IsComplete() const
	{
		Bind();
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			const char* errorMsg = "Unknown error";
			switch (status)
			{
			case GL_FRAMEBUFFER_UNDEFINED:
				errorMsg = "Framebuffer undefined (target is default framebuffer)";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				errorMsg = "Incomplete attachment (texture parameters invalid)";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				errorMsg = "Missing attachment (no color or depth attached)";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				errorMsg = "Incomplete draw buffer";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				errorMsg = "Incomplete read buffer";
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				errorMsg = "Framebuffer format combination not supported";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				errorMsg = "Incomplete multisample (attachment sample counts don't match)";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				errorMsg = "Incomplete layer targets";
				break;
			}

			VP_CORE_ERROR("Framebuffer {}: Not complete - {}", m_FBO, errorMsg);
			return false;
		}

		return true;
	}
}
