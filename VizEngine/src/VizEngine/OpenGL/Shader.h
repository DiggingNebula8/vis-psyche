#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "glm.hpp"
#include "VizEngine/Core.h"

namespace VizEngine
{
	// Struct to return two or more strings. For Vertex and Fragment Shader Programs from the same file.
	struct ShaderPrograms
	{
		std::string VertexProgram;
		std::string FragmentProgram;
	};

	// Shader Class
	class VizEngine_API Shader
	{
	public:
		// Constructor that build the Shader Program
		Shader(const std::string& shaderFile);
		// Destructor
		~Shader();

		// Prevent copying (Rule of 5)
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		// Allow moving
		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		// Binds the Shader Program
		void Bind() const;
		// Unbinds the Shader Program
		void Unbind() const;

		// Utility uniform functions
		void SetBool(const std::string& name, bool value);
		void SetInt(const std::string& name, int value);
		void SetFloat(const std::string& name, float value);
		void SetVec3(const std::string& name, const glm::vec3& value);
		void SetVec4(const std::string& name, const glm::vec4& value);
		void SetColor(const std::string& name, const glm::vec4& value);
		void SetMatrix4fv(const std::string& name, const glm::mat4& matrix);

	private:
		std::string m_shaderPath;
		unsigned int m_RendererID;
		std::unordered_map<std::string, int> m_LocationCache;

		// Shader parser with a return type of ShaderPrograms
		ShaderPrograms ShaderParser(const std::string& shaderFile);
		// Shader compiler
		unsigned int CompileShader(unsigned int type, const std::string& source);
		// Creates the final shader 
		unsigned int CreateShader(const std::string& vert, const std::string& frag);
		// Get uniform location for the set shader uniforms
		int GetUniformLocation(const std::string& name);
		// Utility function for checking shader compilation/linking errors.
		// Returns true on success, false on error.
		bool CheckCompileErrors(unsigned int shader, std::string type);
	};
}