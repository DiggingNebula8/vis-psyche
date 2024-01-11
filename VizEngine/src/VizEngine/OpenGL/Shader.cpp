#include "vepch.h"
#include"Shader.h"

// Reads a .shader file and outputs two strings from the Shader Program Struct
ShaderPrograms Shader::ShaderParser(const std::string& shaderFile)
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream input(shaderFile, std::ios::binary);
	if (input)
	{
		std::string contents;
		std::stringstream ss[2];
		ShaderType shaderType = ShaderType::NONE;
		while (getline(input, contents))
		{
			if (contents.find("#shader") != std::string::npos)
			{
				if (contents.find("vertex") != std::string::npos)
				{
					shaderType = ShaderType::VERTEX;
				}
				else if (contents.find("fragment") != std::string::npos)
				{
					shaderType = ShaderType::FRAGMENT;
				}
			}
			else
			{
				ss[(int)shaderType] << contents << '\n';
			}
		}
		return {ss[0].str(), ss[1].str()};
	}
	throw(errno);
}

// Constructor that builds the final Shader
Shader::Shader(const std::string& shaderFile)
	:m_shaderPath(shaderFile), m_RendererID(0)
{
	// Creating an object of the type ShaderPrograms to read the shader data from the file
	ShaderPrograms shaders = ShaderParser(shaderFile);
	m_RendererID = CreateShader(shaders.VertexProgram, shaders.FragmentProgram);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

// Bind the Shader Program
void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

// Unbind the Shader Program
void Shader::Unbind() const
{
	glUseProgram(0);
}

// compile and create shaders
unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	// Convert the shader source strings into character arrays
	const char* src = source.c_str();

	// Create Vertex Shader Object and get its reference
	unsigned int id = glCreateShader(type);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(id, 1, &src, nullptr);
	// Check and Compile the Vertex Shader into machine code
	glCompileShader(id);

	return id;
}

unsigned int Shader::CreateShader(const std::string& vert, const std::string& frag)
{
	// Create Shader Program Object and get its reference
	unsigned int program = glCreateProgram();

	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vert);
	CheckCompileErrors(vs, "VERTEX: ");

	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, frag);
	CheckCompileErrors(fs, "FRAGMENT: ");

	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(program);
	glValidateProgram(program);
	CheckCompileErrors(program, "LINK:");
	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

// utility uniform functions
void Shader::SetBool(const std::string& name, bool value)
{
	glUniform1i(GetUniformLocation(name), (int)value);
}
void Shader::SetInt(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}
void Shader::SetFloat(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetColor(const std::string& name, const glm::vec4& value)
{
	glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::SetMatrix4fv(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_LocationCache.find(name) != m_LocationCache.end())
		return m_LocationCache[name];

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cout << "Warning: Shader Uniform " << name << " doesn't exist!" << std::endl;

	m_LocationCache[name] = location;
	return location;
}

// utility function for checking shader compilation/linking errors.
void Shader::CheckCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER ERROR::COMPILATION ERROR: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER ERROR::LINKING ERROR: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

