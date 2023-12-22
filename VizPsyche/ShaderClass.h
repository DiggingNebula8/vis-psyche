#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

// Struct to return two or more strings. For Vertex and Fragment Shader Programs from the same file.
struct ShaderPrograms
{
	std::string VertexProgram;
	std::string FragmentProgram;
};

// Shader parser with a return type of ShaderPrograms
static ShaderPrograms ShaderParser(const char* shader);

class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* shader);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();
};
#endif