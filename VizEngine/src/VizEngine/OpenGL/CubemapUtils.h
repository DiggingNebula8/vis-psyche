// VizEngine/src/VizEngine/OpenGL/CubemapUtils.h

#pragma once

#include <memory>
#include "VizEngine/Core.h"

namespace VizEngine
{
	class Texture;

	/**
	 * Utilities for cubemap texture operations.
	 */
	class VizEngine_API CubemapUtils
	{
	public:
		/**
		 * Convert equirectangular HDR texture to cubemap.
		 * Renders the equirectangular map to 6 cubemap faces using a shader.
		 * This is a one-time conversion operation.
		 * @param equirectangularMap Source HDR texture (2:1 aspect ratio)
		 * @param resolution Resolution per cubemap face (e.g., 512, 1024)
		 * @return Cubemap texture ready for use in skybox or IBL
		 */
		static std::shared_ptr<Texture> EquirectangularToCubemap(
			std::shared_ptr<Texture> equirectangularMap,
			int resolution
		);
	};
}
