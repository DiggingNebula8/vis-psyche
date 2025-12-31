#pragma once

#include "VizEngine/Core.h"
#include "VizEngine/OpenGL/Texture.h"
#include "glm.hpp"
#include <memory>
#include <string>

namespace VizEngine
{
	/**
	 * PBR Material using the metallic-roughness workflow (glTF standard).
	 * 
	 * This is separate from the Phong Material in Light.h.
	 * Use this for models loaded from glTF files.
	 */
	struct VizEngine_API PBRMaterial
	{
		std::string Name = "Unnamed";

		// PBR base properties
		glm::vec4 BaseColor = glm::vec4(1.0f);  // RGBA albedo color
		float Metallic = 0.0f;                   // 0 = dielectric, 1 = metal
		float Roughness = 0.5f;                  // 0 = smooth/mirror, 1 = rough

		// Textures (nullptr if not present)
		std::shared_ptr<Texture> BaseColorTexture = nullptr;
		std::shared_ptr<Texture> MetallicRoughnessTexture = nullptr;  // G=roughness, B=metallic
		std::shared_ptr<Texture> NormalTexture = nullptr;
		std::shared_ptr<Texture> OcclusionTexture = nullptr;
		std::shared_ptr<Texture> EmissiveTexture = nullptr;

		// Emissive
		glm::vec3 EmissiveFactor = glm::vec3(0.0f);

		// Alpha mode
		enum class AlphaMode { Opaque, Mask, Blend };
		AlphaMode Alpha = AlphaMode::Opaque;
		float AlphaCutoff = 0.5f;

		// Double-sided rendering
		bool DoubleSided = false;

		PBRMaterial() = default;

		// Simple constructor with just color
		explicit PBRMaterial(const glm::vec4& baseColor)
			: BaseColor(baseColor) {}

		// Constructor with color and metallic/roughness
		PBRMaterial(const glm::vec4& baseColor, float metallic, float roughness)
			: BaseColor(baseColor), Metallic(metallic), Roughness(roughness) {}

		// Helper methods
		bool HasBaseColorTexture() const { return BaseColorTexture != nullptr; }
		bool HasMetallicRoughnessTexture() const { return MetallicRoughnessTexture != nullptr; }
		bool HasNormalTexture() const { return NormalTexture != nullptr; }
		bool HasOcclusionTexture() const { return OcclusionTexture != nullptr; }
		bool HasEmissiveTexture() const { return EmissiveTexture != nullptr; }
		bool HasAnyTexture() const 
		{ 
			return HasBaseColorTexture() || HasMetallicRoughnessTexture() || 
			       HasNormalTexture() || HasOcclusionTexture() || HasEmissiveTexture(); 
		}
	};
}

