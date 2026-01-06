#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 v_TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    v_TexCoords = aPos;
    
    // Remove translation from view matrix (keep rotation only)
    mat4 viewNoTranslation = mat4(mat3(u_View));
    
    vec4 pos = u_Projection * viewNoTranslation * vec4(aPos, 1.0);
    
    // Set depth to maximum (far plane) so skybox renders behind everything
    gl_Position = pos.xyww;  // Equivalent to setting z = w
}


#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 v_TexCoords;

uniform samplerCube u_Skybox;

void main()
{
    // Sample cubemap using 3D direction vector
    vec3 color = texture(u_Skybox, v_TexCoords).rgb;
    
    FragColor = vec4(color, 1.0);
}
