// VizEngine/src/VizEngine/Renderer/Skybox.h

#pragma once

#include <memory>
#include "VizEngine/Core.h"

namespace VizEngine
{
	class Texture;
	class Shader;
	class VertexArray;
	class VertexBuffer;
	class Camera;

	/**
	 * Skybox renderer using cubemap textures.
	 * Renders an environment cube that follows camera rotation but not translation.
	 */
	class VizEngine_API Skybox
	{
	public:
		/**
		 * Create skybox from cubemap texture.
		 * @param cubemap Cubemap texture (must be GL_TEXTURE_CUBE_MAP)
		 */
		Skybox(std::shared_ptr<Texture> cubemap);
		~Skybox() = default;

		/**
		 * Render the skybox.
		 * Call this after rendering the scene (with depth write disabled if needed).
		 * @param camera Camera for view/projection matrices
		 */
		void Render(const Camera& camera);

	private:
		std::shared_ptr<Texture> m_Cubemap;
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::shared_ptr<Shader> m_Shader;

		void SetupMesh();
	};
}
