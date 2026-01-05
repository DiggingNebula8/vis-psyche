// VizEngine/src/VizEngine/OpenGL/Framebuffer.h

#pragma once

#include <memory>
#include "VizEngine/Core.h"

namespace VizEngine
{
	class Texture;

	/**
	 * RAII wrapper for OpenGL framebuffer objects.
	 * Allows rendering to textures instead of the default framebuffer (screen).
	 */
	class VizEngine_API Framebuffer
	{
	public:
		/**
		 * Create a framebuffer with the specified dimensions.
		 * Note: Framebuffer is incomplete until attachments are added.
		 * @param width Width in pixels
		 * @param height Height in pixels
		 */
		Framebuffer(int width, int height);

		/**
		 * Destructor. Deletes the OpenGL framebuffer object.
		 */
		~Framebuffer();

		// Non-copyable
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer& operator=(const Framebuffer&) = delete;

		/**
		 * Bind this framebuffer for rendering.
		 * Subsequent draw calls will render to this framebuffer's attachments.
		 */
		void Bind() const;

		/**
		 * Unbind this framebuffer and return to the default framebuffer.
		 */
		void Unbind() const;

		/**
		 * Attach a color texture to a specific color attachment slot.
		 * @param texture The texture to attach (must match framebuffer dimensions)
		 * @param slot Attachment slot (0 for GL_COLOR_ATTACHMENT0, etc.)
		 */
		void AttachColorTexture(std::shared_ptr<Texture> texture, int slot = 0);

		/**
		 * Attach a depth texture.
		 * @param texture The depth texture to attach (format must be GL_DEPTH_COMPONENT*)
		 */
		void AttachDepthTexture(std::shared_ptr<Texture> texture);

		/**
		 * Check if the framebuffer is complete and ready for rendering.
		 * Call this after adding all attachments.
		 * @return true if complete, false otherwise
		 */
		bool IsComplete() const;

		/**
		 * Get the width of the framebuffer.
		 */
		int GetWidth() const { return m_Width; }

		/**
		 * Get the height of the framebuffer.
		 */
		int GetHeight() const { return m_Height; }

		/**
		 * Get the OpenGL framebuffer ID.
		 */
		unsigned int GetID() const { return m_fbo; }

	private:
		unsigned int m_fbo = 0;
		int m_Width = 0;
		int m_Height = 0;

		// Store references to attached textures to keep them alive
		std::shared_ptr<Texture> m_ColorAttachments[8];  // Up to 8 MRT
		std::shared_ptr<Texture> m_DepthAttachment;
	};
}
