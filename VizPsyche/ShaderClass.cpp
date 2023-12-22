#include"ShaderClass.h"

// Reads a .shader file and outputs two strings from the Shader Program Struct
static ShaderPrograms ShaderParser(const char* shader)
{
	std::ifstream in(shader, std::ios::binary);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	if (in)
	{
		std::string contents;
		std::stringstream ss[2];
		ShaderType shaderType = ShaderType::NONE;
		while (getline(in, contents))
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
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Create Shader Program Object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ID);

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