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
	Material Model::s_DefaultMaterial = Material(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), 0.0f, 0.5f);

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
		// Validate bufferView index
		if (accessor.bufferView < 0 || accessor.bufferView >= static_cast<int>(model.bufferViews.size()))
		{
			VP_CORE_ERROR("Accessor bufferView index {} out of range", accessor.bufferView);
			return nullptr;
		}
		
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		
		// Validate buffer index
		if (bufferView.buffer < 0 || bufferView.buffer >= static_cast<int>(model.buffers.size()))
		{
			VP_CORE_ERROR("BufferView buffer index {} out of range", bufferView.buffer);
			return nullptr;
		}
		
		const auto& buffer = model.buffers[bufferView.buffer];
		
		// Calculate expected element size based on accessor type
		// VEC3 = 3 components, VEC4 = 4 components, VEC2 = 2 components, SCALAR = 1 component
		size_t componentsPerElement = 1;
		switch (accessor.type)
		{
			case TINYGLTF_TYPE_SCALAR: componentsPerElement = 1; break;
			case TINYGLTF_TYPE_VEC2:   componentsPerElement = 2; break;
			case TINYGLTF_TYPE_VEC3:   componentsPerElement = 3; break;
			case TINYGLTF_TYPE_VEC4:   componentsPerElement = 4; break;
			case TINYGLTF_TYPE_MAT2:   componentsPerElement = 4; break;
			case TINYGLTF_TYPE_MAT3:   componentsPerElement = 9; break;
			case TINYGLTF_TYPE_MAT4:   componentsPerElement = 16; break;
		}
		size_t elementSize = componentsPerElement * sizeof(T);
		
		// Check for interleaved data - stride must be 0 (tightly packed) or match element size
		if (bufferView.byteStride != 0 && bufferView.byteStride != elementSize)
		{
			VP_CORE_ERROR("glTF buffer has unsupported byteStride ({}), expected 0 or {}", 
				bufferView.byteStride, elementSize);
			return nullptr;
		}
		
		// Validate that offsets don't exceed buffer size
		size_t totalOffset = bufferView.byteOffset + accessor.byteOffset;
		if (totalOffset > buffer.data.size())
		{
			VP_CORE_ERROR("Buffer offset ({}) exceeds buffer size ({})", totalOffset, buffer.data.size());
			return nullptr;
		}
		
		// Validate that the entire data range is within bounds
		size_t requiredBytes = accessor.count * elementSize;
		if (totalOffset + requiredBytes > buffer.data.size())
		{
			VP_CORE_ERROR("Buffer data range (offset {} + {} bytes) exceeds buffer size ({})", 
				totalOffset, requiredBytes, buffer.data.size());
			return nullptr;
		}
		
		return reinterpret_cast<const T*>(
			buffer.data.data() + totalOffset
		);
	}

	// Helper function to validate buffer bounds for vertex attributes
	static bool ValidateAttributeBuffer(
		const tinygltf::Model& gltfModel,
		const tinygltf::Accessor& accessor,
		size_t vertexCount,
		size_t componentsPerVertex,
		const std::string& attributeName)
	{
		// Validate bufferView index
		if (accessor.bufferView < 0 || accessor.bufferView >= static_cast<int>(gltfModel.bufferViews.size()))
		{
			VP_CORE_WARN("{} bufferView index out of range, skipping attribute", attributeName);
			return false;
		}
		
		const auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
		
		// Validate buffer index
		if (bufferView.buffer < 0 || bufferView.buffer >= static_cast<int>(gltfModel.buffers.size()))
		{
			VP_CORE_WARN("{} buffer index out of range, skipping attribute", attributeName);
			return false;
		}
		
		const auto& buffer = gltfModel.buffers[bufferView.buffer];
		size_t requiredBytes = vertexCount * componentsPerVertex * sizeof(float);
		
		// Check for underflow before calculating available bytes
		size_t totalOffset = bufferView.byteOffset + accessor.byteOffset;
		if (totalOffset > buffer.data.size())
		{
			VP_CORE_WARN("{} buffer offset exceeds buffer size, skipping attribute", attributeName);
			return false;
		}
		
		size_t availableBytes = buffer.data.size() - totalOffset;
		if (requiredBytes > availableBytes)
		{
			VP_CORE_WARN("{} buffer too small, skipping attribute", attributeName);
			return false;
		}
		return true;
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

	const Material& Model::GetMaterialForMesh(size_t meshIndex) const
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
			Material material;
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
				material.Alpha = Material::AlphaMode::Mask;
				material.AlphaCutoff = static_cast<float>(gltfMat.alphaCutoff);
			}
			else if (gltfMat.alphaMode == "BLEND")
			{
				material.Alpha = Material::AlphaMode::Blend;
			}
			else
			{
				material.Alpha = Material::AlphaMode::Opaque;
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

				int posAccessorIndex = primitive.attributes.at("POSITION");
				if (posAccessorIndex < 0 || posAccessorIndex >= static_cast<int>(gltfModel.accessors.size()))
				{
					VP_CORE_ERROR("POSITION accessor index {} out of range, skipping primitive", posAccessorIndex);
					continue;
				}
				const auto& posAccessor = gltfModel.accessors[posAccessorIndex];
				const float* positions = GetBufferData<float>(gltfModel, posAccessor);
				if (!positions)
				{
					VP_CORE_ERROR("Failed to load positions for mesh, skipping primitive");
					continue;
				}
				size_t vertexCount = posAccessor.count;



				const float* normals = nullptr;  // nullptr check deferred to vertex loop
				if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
				{
					int normAccessorIndex = primitive.attributes.at("NORMAL");
					if (normAccessorIndex >= 0 && normAccessorIndex < static_cast<int>(gltfModel.accessors.size()))
					{
						const auto& normAccessor = gltfModel.accessors[normAccessorIndex];
						if (ValidateAttributeBuffer(gltfModel, normAccessor, vertexCount, 3, "Normal"))
						{
							normals = GetBufferData<float>(gltfModel, normAccessor);
						}
					}
					else
					{
						VP_CORE_WARN("NORMAL accessor index {} out of range", normAccessorIndex);
					}
				}

				const float* texCoords = nullptr;  // nullptr check deferred to vertex loop
				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
				{
					int uvAccessorIndex = primitive.attributes.at("TEXCOORD_0");
					if (uvAccessorIndex >= 0 && uvAccessorIndex < static_cast<int>(gltfModel.accessors.size()))
					{
						const auto& uvAccessor = gltfModel.accessors[uvAccessorIndex];
						if (ValidateAttributeBuffer(gltfModel, uvAccessor, vertexCount, 2, "TexCoord"))
						{
							texCoords = GetBufferData<float>(gltfModel, uvAccessor);
						}
					}
					else
					{
						VP_CORE_WARN("TEXCOORD_0 accessor index {} out of range", uvAccessorIndex);
					}
				}

				const float* colors = nullptr;  // nullptr check deferred to vertex loop
				int colorComponents = 0;
				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					int colorAccessorIndex = primitive.attributes.at("COLOR_0");
					if (colorAccessorIndex >= 0 && colorAccessorIndex < static_cast<int>(gltfModel.accessors.size()))
					{
						const auto& colorAccessor = gltfModel.accessors[colorAccessorIndex];
						colorComponents = (colorAccessor.type == TINYGLTF_TYPE_VEC4) ? 4 : 3;
						if (colorAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
						{
							if (ValidateAttributeBuffer(gltfModel, colorAccessor, vertexCount, colorComponents, "Color"))
							{
								colors = GetBufferData<float>(gltfModel, colorAccessor);
							}
						}
						else if (colorAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ||
						         colorAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						{
							VP_CORE_WARN("Normalized integer COLOR_0 not yet supported, using default color");
						}
					}
					else
					{
						VP_CORE_WARN("COLOR_0 accessor index {} out of range", colorAccessorIndex);
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
					if (primitive.indices < static_cast<int>(gltfModel.accessors.size()))
					{
						const auto& indexAccessor = gltfModel.accessors[primitive.indices];
						LoadIndices(gltfModel, indexAccessor, indices);
					}
					else
					{
						VP_CORE_ERROR("Index accessor {} out of range, generating sequential indices", primitive.indices);
						indices.reserve(vertexCount);
						for (size_t i = 0; i < vertexCount; i++)
						{
							indices.push_back(static_cast<unsigned int>(i));
						}
					}
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
		// Validate bufferView index
		if (accessor.bufferView < 0 || accessor.bufferView >= static_cast<int>(gltfModel.bufferViews.size()))
		{
			VP_CORE_ERROR("Index accessor bufferView {} out of range", accessor.bufferView);
			return;
		}
		
		const auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
		
		// Validate buffer index
		if (bufferView.buffer < 0 || bufferView.buffer >= static_cast<int>(gltfModel.buffers.size()))
		{
			VP_CORE_ERROR("Index bufferView buffer {} out of range", bufferView.buffer);
			return;
		}
		
		const auto& buffer = gltfModel.buffers[bufferView.buffer];
		
		// Check offsets don't exceed buffer size to prevent underflow
		size_t totalOffset = static_cast<size_t>(bufferView.byteOffset) + static_cast<size_t>(accessor.byteOffset);
		if (totalOffset > buffer.data.size())
		{
			VP_CORE_ERROR("Index buffer offsets ({}) exceed buffer size ({})", totalOffset, buffer.data.size());
			return;
		}
		
		// Validate buffer bounds based on component type
		size_t componentSize = 0;
		switch (accessor.componentType)
		{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: componentSize = 1; break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: componentSize = 2; break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: componentSize = 4; break;
			default:
				VP_CORE_ERROR("Unsupported index component type: {}", accessor.componentType);
				return;
		}
		
		size_t requiredBytes = accessor.count * componentSize;
		size_t availableBytes = buffer.data.size() - totalOffset;
		if (requiredBytes > availableBytes)
		{
			VP_CORE_ERROR("Index buffer too small for accessor.count");
			return;
		}
		
		// Note: Indices are not interleaved, so byteStride is not checked here
		const void* dataPtr = buffer.data.data() + totalOffset;

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

