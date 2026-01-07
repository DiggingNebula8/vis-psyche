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
uniform sampler2D u_HDRBuffer;
uniform float u_Threshold;  // Typically 1.0 for HDR
uniform float u_Knee;        // Soft threshold range (e.g., 0.1-0.5)

// ============================================================================
// Bright Pass Extraction with Soft Threshold
// ============================================================================

vec3 ExtractBrightRegions(vec3 color)
{
    // Calculate luminance (perceptual brightness)
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    // Quadratic soft threshold (smooth knee)
    // This creates a smooth transition around the threshold instead of a hard cut
    float soft = luminance - u_Threshold + u_Knee;
    soft = clamp(soft, 0.0, 2.0 * u_Knee);
    soft = (soft * soft) / (4.0 * u_Knee + 0.00001);  // +epsilon to prevent divide by zero
    
    // Calculate contribution
    float contribution = max(soft, luminance - u_Threshold);
    contribution /= max(luminance, 0.00001);  // Normalize by luminance
    
    // Return color scaled by contribution (preserves color, only adjusts intensity)
    return color * contribution;
}

void main()
{
    vec3 hdrColor = texture(u_HDRBuffer, v_TexCoords).rgb;
    vec3 brightColor = ExtractBrightRegions(hdrColor);
    
    FragColor = vec4(brightColor, 1.0);
}
