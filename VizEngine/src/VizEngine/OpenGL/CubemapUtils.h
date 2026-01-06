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

		/**
		 * Generate diffuse irradiance cubemap from environment map.
		 * Convolves environment over hemisphere for each direction.
		 * @param environmentMap Source HDR cubemap
		 * @param resolution Resolution per face (32 typical)
		 * @return Irradiance cubemap for diffuse IBL
		 */
		static std::shared_ptr<Texture> GenerateIrradianceMap(
			std::shared_ptr<Texture> environmentMap,
			int resolution = 32
		);

		/**
		 * Generate specular pre-filtered environment map.
		 * Each mip level stores environment convolved for that roughness.
		 * @param environmentMap Source HDR cubemap
		 * @param resolution Base resolution (512 typical)
		 * @return Pre-filtered cubemap with roughness in mip chain
		 */
		static std::shared_ptr<Texture> GeneratePrefilteredMap(
			std::shared_ptr<Texture> environmentMap,
			int resolution = 512
		);

		/**
		 * Generate BRDF integration lookup table.
		 * 2D texture storing Fresnel scale/bias indexed by (NdotV, roughness).
		 * Environment-independent - generate once, use forever.
		 * @param resolution LUT resolution (512 typical)
		 * @return 2D RG texture for BRDF lookup
		 */
		static std::shared_ptr<Texture> GenerateBRDFLUT(int resolution = 512);
	};
}
