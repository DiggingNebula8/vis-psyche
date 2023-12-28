#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

// Struct to return two or more strings. For Vertex and Fragment Shader Programs from the same file.
struct ShaderPrograms
{
	enum class ShaderType;
	std::string VertexProgram;
	std::string FragmentProgram;
};


class Shader
{
private:
	std::string m_shaderPath;
	unsigned int m_RendererID;
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const std::string& shaderFile);
	~Shader();

	// Activates the Shader Program
	void Bind() const;
	// Deletes the Shader Program
	void Unbind() const;

    // utility uniform functions
    void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void SetColor(const std::string& name, float value[4]);


private:
	// Shader parser with a return type of ShaderPrograms
	ShaderPrograms ShaderParser(const std::string& shaderFile);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vert, const std::string& frag);
	unsigned int GetUniformLocation(const std::string& name);
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
	void CheckCompileErrors(unsigned int shader, std::string type);
};
#endif