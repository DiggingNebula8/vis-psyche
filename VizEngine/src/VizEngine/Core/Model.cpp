#include "Model.h"
#include "VizEngine/Log.h"

// tinygltf is header-only, implementation is in TinyGLTF.cpp
// Must match the defines used in TinyGLTF.cpp
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#include "tiny_gltf.h"

#include <filesystem>

namespace VizEngine
{
	// Default material for meshes without one assigned
	PBRMaterial Model::s_DefaultMaterial = PBRMaterial(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), 0.0f, 0.5f);

	//==========================================================================
	// ModelLoader - Internal helper class to keep tinygltf out of header
	//==========================================================================
	class Model::ModelLoader
	{
	public:
		static std::unique_ptr<Model> Load(const std::string& filepath);

	private:
		ModelLoader(Model* model, const std::string& filepath);

		void LoadMaterials(const tinygltf::Model& gltfModel);
		void LoadMeshes(const tinygltf::Model& gltfModel);
		void LoadIndices(const tinygltf::Model& gltfModel,
			const tinygltf::Accessor& accessor,
			std::vector<unsigned int>& indices);
		std::shared_ptr<Texture> LoadTexture(const tinygltf::Model& gltfModel, int textureIndex);

		Model* m_Model;
		std::string m_Directory;
		std::unordered_map<int, std::shared_ptr<Texture>> m_TextureCache;
	};

	//==========================================================================
	// Helper functions
	//==========================================================================
	static std::string GetDirectory(const std::string& filepath)
	{
		std::filesystem::path path(filepath);
		return path.parent_path().string();
	}

	static std::string GetFilename(const std::string& filepath)
	{
		std::filesystem::path path(filepath);
		return path.filename().string();
	}

	static bool EndsWith(const std::string& str, const std::string& suffix)
	{
		if (suffix.size() > str.size()) return false;
		return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	template<typename T>
	static const T* GetBufferData(const tinygltf::Model& model, const tinygltf::Accessor& accessor)
	{
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];
		
		// Fail if byteStride indicates interleaved data we can't handle
		// byteStride of 0 means tightly packed (default), sizeof(T) also works
		if (bufferView.byteStride != 0 && bufferView.byteStride != sizeof(T))
		{
			VP_CORE_ERROR("glTF buffer has unsupported byteStride ({}), cannot load interleaved data", 
				bufferView.byteStride);
			return nullptr;
		}
		
		return reinterpret_cast<const T*>(
			buffer.data.data() + bufferView.byteOffset + accessor.byteOffset
		);
	}

	//==========================================================================
	// Model public interface
	//==========================================================================
	std::unique_ptr<Model> Model::LoadFromFile(const std::string& filepath)
	{
		return ModelLoader::Load(filepath);
	}

	size_t Model::GetMaterialIndexForMesh(size_t meshIndex) const
	{
		if (meshIndex < m_MeshMaterialIndices.size())
		{
			return m_MeshMaterialIndices[meshIndex];
		}
		return 0;
	}

	const PBRMaterial& Model::GetMaterialForMesh(size_t meshIndex) const
	{
		size_t matIndex = GetMaterialIndexForMesh(meshIndex);
		if (matIndex < m_Materials.size())
		{
			return m_Materials[matIndex];
		}
		return s_DefaultMaterial;
	}

	//==========================================================================
	// ModelLoader implementation
	//==========================================================================
	Model::ModelLoader::ModelLoader(Model* model, const std::string& filepath)
		: m_Model(model)
		, m_Directory(GetDirectory(filepath))
	{
	}

	std::unique_ptr<Model> Model::ModelLoader::Load(const std::string& filepath)
	{
		VP_CORE_INFO("Loading model: {}", filepath);

		// Check if file exists first for clearer error messages
		if (!std::filesystem::exists(filepath))
		{
			VP_CORE_ERROR("Model file not found: {}", filepath);
			return nullptr;
		}

		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		// Load based on file extension
		bool success = false;
		if (EndsWith(filepath, ".glb"))
		{
			success = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, filepath);
		}
		else if (EndsWith(filepath, ".gltf"))
		{
			success = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, filepath);
		}
		else
		{
			VP_CORE_ERROR("Unsupported model format: {}", filepath);
			return nullptr;
		}

		if (!warn.empty())
		{
			VP_CORE_WARN("glTF warning: {}", warn);
		}

		if (!err.empty())
		{
			VP_CORE_ERROR("glTF error: {}", err);
		}

		if (!success)
		{
			VP_CORE_ERROR("Failed to load model: {}", filepath);
			return nullptr;
		}

		// Create model instance
		auto model = std::unique_ptr<Model>(new Model());
		model->m_FilePath = filepath;
		model->m_Name = GetFilename(filepath);
		model->m_Directory = GetDirectory(filepath);

		// Use ModelLoader to do the actual loading
		ModelLoader modelLoader(model.get(), filepath);
		modelLoader.LoadMaterials(gltfModel);
		modelLoader.LoadMeshes(gltfModel);

		VP_CORE_INFO("Loaded model '{}': {} meshes, {} materials",
			model->m_Name, model->m_Meshes.size(), model->m_Materials.size());

		return model;
	}

	void Model::ModelLoader::LoadMaterials(const tinygltf::Model& gltfModel)
	{
		for (const auto& gltfMat : gltfModel.materials)
		{
			PBRMaterial material;
			material.Name = gltfMat.name.empty() ? "Material" : gltfMat.name;

			const auto& pbr = gltfMat.pbrMetallicRoughness;

			material.BaseColor = glm::vec4(
				static_cast<float>(pbr.baseColorFactor[0]),
				static_cast<float>(pbr.baseColorFactor[1]),
				static_cast<float>(pbr.baseColorFactor[2]),
				static_cast<float>(pbr.baseColorFactor[3])
			);

			material.Metallic = static_cast<float>(pbr.metallicFactor);
			material.Roughness = static_cast<float>(pbr.roughnessFactor);

			if (pbr.baseColorTexture.index >= 0)
			{
				material.BaseColorTexture = LoadTexture(gltfModel, pbr.baseColorTexture.index);
			}

			if (pbr.metallicRoughnessTexture.index >= 0)
			{
				material.MetallicRoughnessTexture = LoadTexture(gltfModel, pbr.metallicRoughnessTexture.index);
			}

			if (gltfMat.normalTexture.index >= 0)
			{
				material.NormalTexture = LoadTexture(gltfModel, gltfMat.normalTexture.index);
			}

			if (gltfMat.occlusionTexture.index >= 0)
			{
				material.OcclusionTexture = LoadTexture(gltfModel, gltfMat.occlusionTexture.index);
			}

			if (gltfMat.emissiveTexture.index >= 0)
			{
				material.EmissiveTexture = LoadTexture(gltfModel, gltfMat.emissiveTexture.index);
			}
			material.EmissiveFactor = glm::vec3(
				static_cast<float>(gltfMat.emissiveFactor[0]),
				static_cast<float>(gltfMat.emissiveFactor[1]),
				static_cast<float>(gltfMat.emissiveFactor[2])
			);

			if (gltfMat.alphaMode == "MASK")
			{
				material.Alpha = PBRMaterial::AlphaMode::Mask;
				material.AlphaCutoff = static_cast<float>(gltfMat.alphaCutoff);
			}
			else if (gltfMat.alphaMode == "BLEND")
			{
				material.Alpha = PBRMaterial::AlphaMode::Blend;
			}
			else
			{
				material.Alpha = PBRMaterial::AlphaMode::Opaque;
			}

			material.DoubleSided = gltfMat.doubleSided;

			m_Model->m_Materials.push_back(std::move(material));
		}

		if (m_Model->m_Materials.empty())
		{
			m_Model->m_Materials.push_back(Model::s_DefaultMaterial);
		}
	}

	void Model::ModelLoader::LoadMeshes(const tinygltf::Model& gltfModel)
	{
		for (const auto& gltfMesh : gltfModel.meshes)
		{
			for (const auto& primitive : gltfMesh.primitives)
			{
				if (primitive.mode != TINYGLTF_MODE_TRIANGLES && primitive.mode != -1)
				{
					VP_CORE_WARN("Skipping non-triangle primitive in mesh '{}'", gltfMesh.name);
					continue;
				}

				std::vector<Vertex> vertices;
				std::vector<unsigned int> indices;

				if (primitive.attributes.find("POSITION") == primitive.attributes.end())
				{
					VP_CORE_ERROR("Mesh primitive missing POSITION attribute");
					continue;
				}

				const auto& posAccessor = gltfModel.accessors[primitive.attributes.at("POSITION")];
				const float* positions = GetBufferData<float>(gltfModel, posAccessor);
				if (!positions)
				{
					VP_CORE_ERROR("Failed to load positions for mesh, skipping primitive");
					continue;
				}
				size_t vertexCount = posAccessor.count;

				// Validate buffer bounds to prevent overrun from malformed files
				{
					const auto& posBufferView = gltfModel.bufferViews[posAccessor.bufferView];
					const auto& posBuffer = gltfModel.buffers[posBufferView.buffer];
					size_t requiredBytes = vertexCount * 3 * sizeof(float);
					size_t availableBytes = posBuffer.data.size() - posBufferView.byteOffset - posAccessor.byteOffset;
					if (requiredBytes > availableBytes)
					{
						VP_CORE_ERROR("Position buffer too small for accessor.count, skipping primitive");
						continue;
					}
				}

				const float* normals = nullptr;  // nullptr check deferred to vertex loop
				if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
				{
					const auto& normAccessor = gltfModel.accessors[primitive.attributes.at("NORMAL")];
					normals = GetBufferData<float>(gltfModel, normAccessor);
				}

				const float* texCoords = nullptr;  // nullptr check deferred to vertex loop
				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
				{
					const auto& uvAccessor = gltfModel.accessors[primitive.attributes.at("TEXCOORD_0")];
					texCoords = GetBufferData<float>(gltfModel, uvAccessor);
				}

				const float* colors = nullptr;  // nullptr check deferred to vertex loop
				int colorComponents = 0;
				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const auto& colorAccessor = gltfModel.accessors[primitive.attributes.at("COLOR_0")];
					colorComponents = (colorAccessor.type == TINYGLTF_TYPE_VEC4) ? 4 : 3;
					if (colorAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
					{
						colors = GetBufferData<float>(gltfModel, colorAccessor);
					}
					else if (colorAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ||
					         colorAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						VP_CORE_WARN("Normalized integer COLOR_0 not yet supported, using default color");
					}
				}

				vertices.reserve(vertexCount);
				for (size_t i = 0; i < vertexCount; i++)
				{
					Vertex v;

					v.Position = glm::vec4(
						positions[i * 3 + 0],
						positions[i * 3 + 1],
						positions[i * 3 + 2],
						1.0f
					);

					if (normals)
					{
						v.Normal = glm::vec3(
							normals[i * 3 + 0],
							normals[i * 3 + 1],
							normals[i * 3 + 2]
						);
					}
					else
					{
						v.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
					}

					if (texCoords)
					{
						v.TexCoords = glm::vec2(
							texCoords[i * 2 + 0],
							texCoords[i * 2 + 1]
						);
					}
					else
					{
						v.TexCoords = glm::vec2(0.0f);
					}

					if (colors)
					{
						v.Color = glm::vec4(
							colors[i * colorComponents + 0],
							colors[i * colorComponents + 1],
							colors[i * colorComponents + 2],
							colorComponents == 4 ? colors[i * colorComponents + 3] : 1.0f
						);
					}
					else
					{
						v.Color = glm::vec4(1.0f);
					}

					vertices.push_back(v);
				}

				if (primitive.indices >= 0)
				{
					const auto& indexAccessor = gltfModel.accessors[primitive.indices];
					LoadIndices(gltfModel, indexAccessor, indices);
				}
				else
				{
					indices.reserve(vertexCount);
					for (size_t i = 0; i < vertexCount; i++)
					{
						indices.push_back(static_cast<unsigned int>(i));
					}
				}

				auto mesh = std::make_shared<Mesh>(vertices, indices);
				m_Model->m_Meshes.push_back(mesh);

				size_t materialIndex = 0;
				if (primitive.material >= 0)
				{
					size_t matIdx = static_cast<size_t>(primitive.material);
					if (matIdx < m_Model->m_Materials.size())
					{
						materialIndex = matIdx;
					}
					else
					{
						VP_CORE_WARN("Material index {} out of bounds, using default", matIdx);
					}
				}
				m_Model->m_MeshMaterialIndices.push_back(materialIndex);
			}
		}
	}

	void Model::ModelLoader::LoadIndices(const tinygltf::Model& gltfModel,
		const tinygltf::Accessor& accessor,
		std::vector<unsigned int>& indices)
	{
		const auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
		const auto& buffer = gltfModel.buffers[bufferView.buffer];
		// Note: Indices are not interleaved, so byteStride is not checked here
		const void* dataPtr = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

		indices.reserve(accessor.count);

		switch (accessor.componentType)
		{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			{
				const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
				for (size_t i = 0; i < accessor.count; i++)
					indices.push_back(static_cast<unsigned int>(buf[i]));
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			{
				const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
				for (size_t i = 0; i < accessor.count; i++)
					indices.push_back(static_cast<unsigned int>(buf[i]));
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			{
				const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
				for (size_t i = 0; i < accessor.count; i++)
					indices.push_back(buf[i]);
				break;
			}
			default:
				VP_CORE_ERROR("Unsupported index component type: {}", accessor.componentType);
				break;
		}
	}

	std::shared_ptr<Texture> Model::ModelLoader::LoadTexture(const tinygltf::Model& gltfModel, int textureIndex)
	{
		if (textureIndex < 0 || textureIndex >= static_cast<int>(gltfModel.textures.size()))
		{
			return nullptr;
		}

		if (m_TextureCache.find(textureIndex) != m_TextureCache.end())
		{
			return m_TextureCache[textureIndex];
		}

		const auto& texture = gltfModel.textures[textureIndex];

		if (texture.source < 0 || texture.source >= static_cast<int>(gltfModel.images.size()))
		{
			return nullptr;
		}

		const auto& image = gltfModel.images[texture.source];
		std::shared_ptr<Texture> tex;

		if (!image.image.empty())
		{
			tex = std::make_shared<Texture>(
				image.image.data(),
				image.width,
				image.height,
				image.component
			);
			VP_CORE_TRACE("Loaded embedded texture: {}x{}", image.width, image.height);
		}
		else if (!image.uri.empty())
		{
			std::string fullPath = m_Directory.empty()
				? image.uri
				: m_Directory + "/" + image.uri;
			tex = std::make_shared<Texture>(fullPath);
			VP_CORE_TRACE("Loaded external texture: {}", image.uri);
		}

		if (tex)
		{
			m_TextureCache[textureIndex] = tex;
		}

		return tex;
	}
}
