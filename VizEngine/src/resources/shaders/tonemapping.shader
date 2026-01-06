#shader vertex
#version 460 core

// Fullscreen quad (NDC coordinates)
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 v_TexCoords;

void main()
{
    v_TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}


#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 v_TexCoords;

// ============================================================================
// Uniforms
// ============================================================================
uniform sampler2D u_HDRBuffer;
uniform int u_ToneMappingMode;  // 0=Reinhard, 1=ReinhardExt, 2=Exposure, 3=ACES, 4=Uncharted2
uniform float u_Exposure;       // For exposure-based and manual control
uniform float u_Gamma;          // Typically 2.2
uniform float u_WhitePoint;     // For Reinhard Extended

// ============================================================================
// Tone Mapping Functions
// ============================================================================

// ----------------------------------------------------------------------------
// 1. Reinhard (Simple)
// ----------------------------------------------------------------------------
vec3 ReinhardToneMapping(vec3 color)
{
    return color / (color + vec3(1.0));
}

// ----------------------------------------------------------------------------
// 2. Reinhard Extended (White Point Control)
// ----------------------------------------------------------------------------
vec3 ReinhardExtendedToneMapping(vec3 color, float whitePoint)
{
    vec3 numerator = color * (1.0 + (color / (whitePoint * whitePoint)));
    return numerator / (1.0 + color);
}

// ----------------------------------------------------------------------------
// 3. Exposure-Based (Photographer's)
// ----------------------------------------------------------------------------
vec3 ExposureToneMapping(vec3 color, float exposure)
{
    return vec3(1.0) - exp(-color * exposure);
}

// ----------------------------------------------------------------------------
// 4. ACES Filmic (Narkowicz 2016 Approximation)
// Industry standard, used by Unreal Engine and Unity
// ----------------------------------------------------------------------------
vec3 ACESFilmic(vec3 color)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

// ----------------------------------------------------------------------------
// 5. Uncharted 2 (Hable)
// Complex shoulder function with excellent highlight preservation
// ----------------------------------------------------------------------------
vec3 Uncharted2ToneMapping(vec3 color)
{
    float A = 0.15;  // Shoulder strength
    float B = 0.50;  // Linear strength
    float C = 0.10;  // Linear angle
    float D = 0.20;  // Toe strength
    float E = 0.02;  // Toe numerator
    float F = 0.30;  // Toe denominator
    float W = 11.2;  // Linear white point
    
    // Apply curve to color
    vec3 curr = ((color * (A * color + C * B) + D * E) / 
                 (color * (A * color + B) + D * F)) - E / F;
    
    // Apply curve to white point
    float white = ((W * (A * W + C * B) + D * E) / 
                   (W * (A * W + B) + D * F)) - E / F;
    
    return curr / white;
}

// ============================================================================
// Main Fragment Shader
// ============================================================================
void main()
{
    // Sample HDR color from framebuffer
    vec3 hdrColor = texture(u_HDRBuffer, v_TexCoords).rgb;
    
    // Apply exposure (for all modes except Reinhard simple)
    vec3 exposedColor = hdrColor * u_Exposure;
    
    // Apply tone mapping based on selected mode
    vec3 ldrColor;
    
    if (u_ToneMappingMode == 0)
    {
        // Reinhard (simple, no exposure)
        ldrColor = ReinhardToneMapping(hdrColor);
    }
    else if (u_ToneMappingMode == 1)
    {
        // Reinhard Extended (with white point)
        ldrColor = ReinhardExtendedToneMapping(exposedColor, u_WhitePoint);
    }
    else if (u_ToneMappingMode == 2)
    {
        // Exposure-based
        ldrColor = ExposureToneMapping(hdrColor, u_Exposure);
    }
    else if (u_ToneMappingMode == 3)
    {
        // ACES Filmic (recommended)
        ldrColor = ACESFilmic(exposedColor);
    }
    else if (u_ToneMappingMode == 4)
    {
        // Uncharted 2
        ldrColor = Uncharted2ToneMapping(exposedColor);
    }
    else
    {
        // Fallback: no tone mapping (for debugging)
        ldrColor = clamp(exposedColor, 0.0, 1.0);
    }
    
    // Apply gamma correction (linear -> sRGB)
    vec3 srgbColor = pow(ldrColor, vec3(1.0 / u_Gamma));
    
    FragColor = vec4(srgbColor, 1.0);
}
