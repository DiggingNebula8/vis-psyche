#include "Shader.h"
#include "VizEngine/Log.h"
#include <stdexcept>

namespace VizEngine
{
	// Reads a .shader file and outputs two strings from the Shader Program Struct
	ShaderPrograms Shader::ShaderParser(const std::string& shaderFile)
	{
		enum class ShaderType
		{
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		std::ifstream input(shaderFile, std::ios::binary);
		if (!input)
		{
			VP_CORE_ERROR("Failed to open shader file: {}", shaderFile);
			return {"", ""};
		}

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
				if (shaderType != ShaderType::NONE)
				{
					ss[static_cast<int>(shaderType)] << contents << '\n';
				}
			}
		}
		return {ss[0].str(), ss[1].str()};
	}

	// Constructor that builds the final Shader
	Shader::Shader(const std::string& shaderFile)
		: m_shaderPath(shaderFile), m_program(0)
	{
		// Parse the shader file
		ShaderPrograms shaders = ShaderParser(shaderFile);
		if (shaders.VertexProgram.empty() || shaders.FragmentProgram.empty())
		{
			VP_CORE_ERROR("Failed to parse shader file: {}", shaderFile);
			throw std::runtime_error("Failed to parse shader: " + shaderFile);
		}
		
		// Compile and link
		m_program = CreateShader(shaders.VertexProgram, shaders.FragmentProgram);
		if (m_program == 0)
		{
			VP_CORE_ERROR("Failed to compile/link shader: {}", shaderFile);
			throw std::runtime_error("Failed to compile shader: " + shaderFile);
		}
	}

	Shader::~Shader()
	{
		if (m_program != 0)
		{
			glDeleteProgram(m_program);
		}
	}

	// Move constructor
	Shader::Shader(Shader&& other) noexcept
		: m_shaderPath(std::move(other.m_shaderPath)),
		  m_program(other.m_program),
		  m_LocationCache(std::move(other.m_LocationCache))
	{
		other.m_program = 0;
	}

	// Move assignment operator
	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this != &other)
		{
			if (m_program != 0)
			{
				glDeleteProgram(m_program);
			}
			m_shaderPath = std::move(other.m_shaderPath);
			m_program = other.m_program;
			m_LocationCache = std::move(other.m_LocationCache);
			other.m_program = 0;
		}
		return *this;
	}

	// Bind the Shader Program
	void Shader::Bind() const
	{
		glUseProgram(m_program);
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
		if (!CheckCompileErrors(vs, "VERTEX"))
		{
			glDeleteShader(vs);
			glDeleteProgram(program);
			return 0;
		}

		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, frag);
		if (!CheckCompileErrors(fs, "FRAGMENT"))
		{
			glDeleteShader(vs);
			glDeleteShader(fs);
			glDeleteProgram(program);
			return 0;
		}

		// Attach the Vertex and Fragment Shaders to the Shader Program
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		// Wrap-up/Link all the shaders together into the Shader Program
		glLinkProgram(program);
		glValidateProgram(program);

		// Cleanup shader objects (attached to program, no longer needed)
		glDeleteShader(vs);
		glDeleteShader(fs);

		if (!CheckCompileErrors(program, "PROGRAM"))
		{
			glDeleteProgram(program);
			return 0;
		}

		return program;
	}

	// utility uniform functions
	void Shader::SetBool(const std::string& name, bool value)
	{
		glUniform1i(GetUniformLocation(name), static_cast<int>(value));
	}

	void Shader::SetInt(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetVec3(const std::string& name, const glm::vec3& value)
	{
		glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void Shader::SetVec4(const std::string& name, const glm::vec4& value)
	{
		glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
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

		int location = glGetUniformLocation(m_program, name.c_str());
		if (location == -1)
			VP_CORE_WARN("Shader Uniform {} doesn't exist!", name);

		m_LocationCache[name] = location;
		return location;
	}

	// utility function for checking shader compilation/linking errors.
	// Returns true on success, false on error.
	bool Shader::CheckCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				VP_CORE_ERROR("SHADER COMPILATION ERROR ({}): {}", type, infoLog);
				return false;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				VP_CORE_ERROR("SHADER LINKING ERROR ({}): {}", type, infoLog);
				return false;
			}
		}
		return true;
	}
}
