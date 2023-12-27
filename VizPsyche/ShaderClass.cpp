#include"ShaderClass.h"

// Reads a .shader file and outputs two strings from the Shader Program Struct
static ShaderPrograms ShaderParser(const char* shader)
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream input(shader, std::ios::binary);
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
Shader::Shader(const char* shader)
{
	// Creating an object of the type ShaderPrograms to read the shader data from the file
	ShaderPrograms shaders = ShaderParser(shader);

	// Read vertex and fragment shaders from the shader programs and store the strings
	std::string vertexCode = shaders.VertexProgram;
	std::string fragmentCode = shaders.FragmentProgram;

	// Convert the shader source strings into character arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Check and Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);
	CheckCompileErrors(vertexShader, "VERTEX:");


	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Check and Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);
	CheckCompileErrors(fragmentShader, "FRAGMENT:");

	// Create Shader Program Object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ID);
	CheckCompileErrors(ID, "LINK:");
	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

// Activates the Shader Program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}

// utility uniform functions
	// ------------------------------------------------------------------------
void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetColor(const std::string& name, float value[4]) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2], value[3]);
}

// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
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
