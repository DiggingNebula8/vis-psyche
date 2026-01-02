#include "Scene.h"
#include <glad/glad.h>

namespace VizEngine
{
	SceneObject& Scene::Add(std::shared_ptr<Mesh> mesh, const std::string& name)
	{
		SceneObject obj;
		obj.MeshPtr = mesh;
		obj.ObjectTransform = Transform{};
		obj.Color = glm::vec4(1.0f);
		obj.Active = true;
		obj.Name = name;

		m_Objects.push_back(std::move(obj));
		return m_Objects.back();
	}

	void Scene::Remove(size_t index)
	{
		if (index < m_Objects.size())
		{
			m_Objects.erase(m_Objects.begin() + static_cast<std::ptrdiff_t>(index));
		}
	}

	void Scene::Clear()
	{
		m_Objects.clear();
	}

	void Scene::Update(float deltaTime)
	{
		// Placeholder for future animation/physics updates
		// For now, this can be used by applications to implement custom update logic
		(void)deltaTime; // Suppress unused parameter warning
	}

	void Scene::Render(Renderer& renderer, Shader& shader, const Camera& camera)
	{
		shader.Bind();

		for (auto& obj : m_Objects)
		{
			// Skip inactive or invalid objects
			if (!obj.Active) continue;
			if (!obj.MeshPtr) continue;

			// Calculate matrices
			glm::mat4 model = obj.ObjectTransform.GetModelMatrix();
			glm::mat4 mvp = camera.GetViewProjectionMatrix() * model;

			// Set uniforms - both MVP and Model (for lighting)
			shader.SetMatrix4fv("u_MVP", mvp);
			shader.SetMatrix4fv("u_Model", model);
			shader.SetVec4("u_ObjectColor", obj.Color);
			shader.SetColor("u_Color", obj.Color);  // Legacy support
			shader.SetFloat("u_Shininess", obj.Shininess);

			// Draw the object
			// Bind per-object texture if available
			if (obj.TexturePtr)
			{
				obj.TexturePtr->Bind();
			}
			else
			{
				// Unbind texture for objects without textures
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			obj.MeshPtr->Bind();
			renderer.Draw(obj.MeshPtr->GetVertexArray(), obj.MeshPtr->GetIndexBuffer(), shader);
		}
	}
}

