#shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main()
{
    v_TexCoords = a_TexCoords;
    gl_Position = vec4(a_Position, 1.0);
}


#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 v_TexCoords;

// ============================================================================
// Uniforms
// ============================================================================
uniform sampler2D u_Image;
uniform bool u_Horizontal;      // true = horizontal pass, false = vertical
uniform vec2 u_TextureSize;     // For calculating pixel offsets

// ============================================================================
// Gaussian Blur (5-tap separable)
// ============================================================================

void main()
{
    // Gaussian weights for 5-tap kernel (sigma ≈ 1.0)
    // Normalized to sum to 1.0 (energy-conserving)
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    
    // Pixel size in texture coordinates [0,1]
    vec2 texelSize = 1.0 / u_TextureSize;
    
    // Sample center pixel
    vec3 result = texture(u_Image, v_TexCoords).rgb * weights[0];
    
    // Sample neighboring pixels
    if (u_Horizontal)
    {
        // Horizontal blur (vary x, keep y constant)
        for (int i = 1; i < 5; ++i)
        {
            vec2 offset = vec2(texelSize.x * float(i), 0.0);
            result += texture(u_Image, v_TexCoords + offset).rgb * weights[i];
            result += texture(u_Image, v_TexCoords - offset).rgb * weights[i];
        }
    }
    else
    {
        // Vertical blur (vary y, keep x constant)
        for (int i = 1; i < 5; ++i)
        {
            vec2 offset = vec2(0.0, texelSize.y * float(i));
            result += texture(u_Image, v_TexCoords + offset).rgb * weights[i];
            result += texture(u_Image, v_TexCoords - offset).rgb * weights[i];
        }
    }
    
    FragColor = vec4(result, 1.0);
}
