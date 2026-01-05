#shader vertex
#version 460 core

layout(location = 0) in vec4 aPos;

uniform mat4 u_LightSpaceMatrix;  // Light's projection * view
uniform mat4 u_Model;              // Model matrix

void main()
{
    // Transform vertex to light's clip space
    gl_Position = u_LightSpaceMatrix * u_Model * aPos;
}


#shader fragment
#version 460 core

void main()
{
    // Fragment shader can be empty
    // Depth is written automatically to GL_DEPTH_ATTACHMENT
    // No color output needed for depth-only pass
}
