#include "VizEngine/Core/Material.h"
#include "VizEngine/OpenGL/Shader.h"

namespace VizEngine
{
	void Material::Bind(Shader& shader) const
	{
		// Upload PBR material uniforms to shader
		shader.SetVec3("u_Albedo", glm::vec3(BaseColor));
		shader.SetFloat("u_Metallic", Metallic);
		shader.SetFloat("u_Roughness", Roughness);
		shader.SetFloat("u_AO", AO);
	}
}

