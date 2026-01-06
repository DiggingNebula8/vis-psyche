#shader vertex
#version 460 core

// Match existing Mesh vertex layout (see Mesh.cpp SetupMesh)
layout(location = 0) in vec4 aPos;       // Position (vec4)
layout(location = 1) in vec3 aNormal;    // Normal (vec3)
layout(location = 2) in vec4 aColor;     // Color (vec4) - unused in PBR but must be declared
layout(location = 3) in vec2 aTexCoords; // TexCoords (vec2)

out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    // Transform position to world space
    v_WorldPos = vec3(u_Model * aPos);
    
    // Transform normal to world space (use normal matrix for non-uniform scaling)
    v_Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    
    // Pass through texture coordinates
    v_TexCoords = aTexCoords;
    
    gl_Position = u_Projection * u_View * vec4(v_WorldPos, 1.0);
}


#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 v_WorldPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

// ============================================================================
// Material Parameters
// ============================================================================
uniform vec3 u_Albedo;           // Base color (or tint if using texture)
uniform float u_Metallic;        // 0 = dielectric, 1 = metal
uniform float u_Roughness;       // 0 = smooth, 1 = rough
uniform float u_AO;              // Ambient occlusion

// Albedo/Base color texture
uniform sampler2D u_AlbedoTexture;
uniform bool u_UseAlbedoTexture;

// ============================================================================
// Camera
// ============================================================================
uniform vec3 u_ViewPos;

// ============================================================================
// Lights (up to 4 point lights)
// ============================================================================
uniform vec3 u_LightPositions[4];
uniform vec3 u_LightColors[4];
uniform int u_LightCount;

// ============================================================================
// Directional Light (optional, for unified lighting with existing scene)
// ============================================================================
uniform vec3 u_DirLightDirection;   // Direction FROM light (normalized)
uniform vec3 u_DirLightColor;       // Radiance (intensity baked in)
uniform bool u_UseDirLight;         // Enable directional light

// ============================================================================
// Constants
// ============================================================================
const float PI = 3.14159265359;

// ============================================================================
// PBR Helper Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Normal Distribution Function: GGX/Trowbridge-Reitz
// Approximates the proportion of microfacets aligned with halfway vector H.
// 
// Formula: D = α² / (π * ((n·h)²(α² - 1) + 1)²)
// Where α = roughness²
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;        // α = roughness² (perceptual mapping)
    float a2 = a * a;                       // α²
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / denom;
}

// ----------------------------------------------------------------------------
// Geometry Function: Schlick-GGX (single direction)
// Models self-shadowing of microfacets.
// 
// Formula: G1 = (n·v) / ((n·v)(1 - k) + k)
// Where k = (roughness + 1)² / 8 for direct lighting
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;    // k for direct lighting
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

// ----------------------------------------------------------------------------
// Geometry Function: Smith (combined shadowing and masking)
// Combines view and light direction geometry terms.
// 
// Formula: G = G1(n, v) * G1(n, l)
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);  // View direction (masking)
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);  // Light direction (shadowing)
    
    return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------
// Fresnel Equation: Schlick Approximation
// Models how reflectivity increases at grazing angles.
// 
// Formula: F = F0 + (1 - F0)(1 - cosθ)^5
// Where F0 = reflectance at normal incidence
// ----------------------------------------------------------------------------
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ============================================================================
// Main Fragment Shader
// ============================================================================
void main()
{
    // Normalize interpolated vectors
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(u_ViewPos - v_WorldPos);
    
    // Get albedo from texture or uniform
    vec3 albedo = u_Albedo;
    if (u_UseAlbedoTexture)
    {
        vec4 texColor = texture(u_AlbedoTexture, v_TexCoords);
        albedo = texColor.rgb * u_Albedo;  // Multiply texture with tint color
    }
    
    // Calculate F0 (base reflectivity)
    // Dielectrics: 0.04 (approximately 4% reflectivity)
    // Metals: use albedo as F0 (tinted reflections)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, u_Metallic);
    
    // Accumulate radiance from all lights
    vec3 Lo = vec3(0.0);
    
    for (int i = 0; i < u_LightCount; ++i)
    {
        // ====================================================================
        // Per-Light Calculations
        // ====================================================================
        
        // Light direction and distance
        vec3 L = normalize(u_LightPositions[i] - v_WorldPos);
        vec3 H = normalize(V + L);  // Halfway vector
        float distance = length(u_LightPositions[i] - v_WorldPos);
        
        // Attenuation (inverse square law)
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_LightColors[i] * attenuation;
        
        // ====================================================================
        // Cook-Torrance BRDF
        // ====================================================================
        
        // D: Normal Distribution Function (microfacet alignment)
        float D = DistributionGGX(N, H, u_Roughness);
        
        // F: Fresnel (angle-dependent reflectivity)
        float cosTheta = max(dot(H, V), 0.0);
        vec3 F = FresnelSchlick(cosTheta, F0);
        
        // G: Geometry (self-shadowing/masking)
        float G = GeometrySmith(N, V, L, u_Roughness);
        
        // Specular BRDF: (D * F * G) / (4 * NdotV * NdotL)
        vec3 numerator = D * F * G;
        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float denominator = 4.0 * NdotV * NdotL + 0.0001;  // Avoid divide by zero
        vec3 specular = numerator / denominator;
        
        // ====================================================================
        // Diffuse Component (Lambertian)
        // ====================================================================
        
        // kS = Fresnel (specular contribution)
        // kD = 1 - kS (diffuse contribution, energy conservation)
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        
        // Metals have no diffuse (all energy goes to specular)
        kD *= (1.0 - u_Metallic);
        
        // Lambertian diffuse: albedo / π
        vec3 diffuse = kD * albedo / PI;
        
        // ====================================================================
        // Combine and Accumulate
        // ====================================================================
        Lo += (diffuse + specular) * radiance * NdotL;
    }
    
    // ========================================================================
    // Directional Light Contribution (if enabled)
    // ========================================================================
    if (u_UseDirLight)
    {
        // Direction TO light (negate the uniform which is FROM light)
        vec3 L = normalize(-u_DirLightDirection);
        vec3 H = normalize(V + L);
        
        // No attenuation for directional lights (infinitely far)
        vec3 radiance = u_DirLightColor;
        
        // Cook-Torrance BRDF (same as point lights)
        float D = DistributionGGX(N, H, u_Roughness);
        float cosTheta = max(dot(H, V), 0.0);
        vec3 F = FresnelSchlick(cosTheta, F0);
        float G = GeometrySmith(N, V, L, u_Roughness);
        
        vec3 numerator = D * F * G;
        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float denominator = 4.0 * NdotV * NdotL + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - u_Metallic);
        vec3 diffuse = kD * albedo / PI;
        
        Lo += (diffuse + specular) * radiance * NdotL;
    }
    
    // ========================================================================
    // Ambient Lighting (simple constant term for now)
    // Chapter 34 will replace this with Image-Based Lighting
    // ========================================================================
    vec3 ambient = vec3(0.03) * albedo * u_AO;
    
    vec3 color = ambient + Lo;
    
    // ========================================================================
    // Tone Mapping and Gamma Correction
    // ========================================================================
    
    // Reinhard tone mapping (simple, will be improved in Chapter 35)
    color = color / (color + vec3(1.0));
    
    // Gamma correction (linear -> sRGB)
    color = pow(color, vec3(1.0 / 2.2));
    
    FragColor = vec4(color, 1.0);
}
