#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 v_WorldPos;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    v_WorldPos = aPos;
    gl_Position = u_Projection * u_View * vec4(aPos, 1.0);
}


#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 v_WorldPos;

uniform sampler2D u_EquirectangularMap;

// Constants for spherical coordinate normalization
const float INV_TWO_PI = 0.1591549;  // 1 / (2π)
const float INV_PI = 0.3183099;      // 1 / π

// Convert 3D direction vector to equirectangular UV coordinates
vec2 SampleSphericalMap(vec3 v)
{
    // Inverse of spherical coordinates
    // atan(y, x) gives angle in XZ plane (longitude)
    // asin(z) gives angle from equator (latitude)
    
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    
    // Normalize from [-π, π] × [-π/2, π/2] to [0, 1] × [0, 1]
    uv *= vec2(INV_TWO_PI, INV_PI);
    uv += 0.5;
    
    return uv;
}

void main()
{
    // Convert fragment's 3D position to spherical UV
    vec2 uv = SampleSphericalMap(normalize(v_WorldPos));
    
    // Sample equirectangular map
    vec3 color = texture(u_EquirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}
