#include "Scene.h"

namespace VizEngine
{
	SceneObject& Scene::AddObject(std::shared_ptr<Mesh> mesh, const std::string& name)
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

	void Scene::RemoveObject(size_t index)
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

			// Calculate MVP matrix for this object
			glm::mat4 mvp = camera.GetViewProjectionMatrix() * obj.ObjectTransform.GetModelMatrix();

			// Set uniforms
			shader.SetMatrix4fv("u_MVP", mvp);
			shader.SetColor("u_Color", obj.Color);

			// Draw the object
			obj.MeshPtr->Bind();
			renderer.Draw(obj.MeshPtr->GetVertexArray(), obj.MeshPtr->GetIndexBuffer(), shader);
		}
	}
}

