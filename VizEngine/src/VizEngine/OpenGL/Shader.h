#pragma once

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include<unordered_map>
#include"glm.hpp"

// Struct to return two or more strings. For Vertex and Fragment Shader Programs from the same file.
struct ShaderPrograms
{
	enum class ShaderType;
	std::string VertexProgram;
	std::string FragmentProgram;
};

// Shader Class
class Shader
{
private:
	std::string m_shaderPath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_LocationCache;
public:
	// Constructor that build the Shader Program
	Shader(const std::string& shaderFile);
	// Destructor
	~Shader();

	// Binds the Shader Program
	void Bind() const;
	// Unvinds the Shader Program
	void Unbind() const;

    // utility uniform functions
    void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void SetColor(const std::string& name, const glm::vec4& value);
	void SetMatrix4f(const std::string& name, const glm::mat4& matrix);


private:
	// Shader parser with a return type of ShaderPrograms
	ShaderPrograms ShaderParser(const std::string& shaderFile);
	// Shader compiler
	unsigned int CompileShader(unsigned int type, const std::string& source);
	// Creates the final shader 
	unsigned int CreateShader(const std::string& vert, const std::string& frag);
	// Get uniform location for the set shader uniforms
	int GetUniformLocation(const std::string& name);
    // utility function for checking shader compilation/linking errors.
	void CheckCompileErrors(unsigned int shader, std::string type);
};