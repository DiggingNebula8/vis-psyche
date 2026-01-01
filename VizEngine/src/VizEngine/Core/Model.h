#pragma once

#include "VizEngine/Core.h"
#include "VizEngine/Core/Mesh.h"
#include "VizEngine/Core/Material.h"
#include "VizEngine/OpenGL/Texture.h"
#include "glm.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace VizEngine
{
	/**
	 * Model represents a loaded 3D model file (glTF/GLB).
	 * 
	 * A model can contain multiple meshes and materials.
	 * Use Model::LoadFromFile() to load models.
	 * 
	 * Example:
	 *   auto model = Model::LoadFromFile("assets/helmet.glb");
	 *   for (auto& mesh : model->GetMeshes()) {
	 *       scene.Add(mesh, name);
	 *   }
	 */
	class VizEngine_API Model
	{
	public:
		/**
		 * Load a model from a glTF or GLB file.
		 * Returns nullptr on failure.
		 */
		static std::unique_ptr<Model> LoadFromFile(const std::string& filepath);

		~Model() = default;

		// Prevent copying (models can be large)
		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		// Allow moving
		Model(Model&&) noexcept = default;
		Model& operator=(Model&&) noexcept = default;

		// Access loaded data
		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; }
		const std::vector<PBRMaterial>& GetMaterials() const { return m_Materials; }

		// Get the material index for a specific mesh
		size_t GetMaterialIndexForMesh(size_t meshIndex) const;
		
		// Get the material for a specific mesh (convenience)
		const PBRMaterial& GetMaterialForMesh(size_t meshIndex) const;

		// Model info
		const std::string& GetName() const { return m_Name; }
		const std::string& GetFilePath() const { return m_FilePath; }
		size_t GetMeshCount() const { return m_Meshes.size(); }
		size_t GetMaterialCount() const { return m_Materials.size(); }

		// Check if model loaded successfully
		bool IsValid() const { return !m_Meshes.empty(); }

	private:
		// Only LoadFromFile can create Models
		Model() = default;

		// Internal loading implementation (defined in cpp file)
		// We use a pimpl-like pattern to keep tinygltf out of the header
		class ModelLoader;
		friend class ModelLoader;

		std::string m_Name;
		std::string m_FilePath;
		std::string m_Directory;  // For resolving relative texture paths

		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		std::vector<PBRMaterial> m_Materials;
		std::vector<size_t> m_MeshMaterialIndices;  // Material index for each mesh

		// Texture cache to avoid reloading same texture
		std::unordered_map<int, std::shared_ptr<Texture>> m_TextureCache;

		// Default material for meshes without one
		static PBRMaterial s_DefaultMaterial;
	};
}

