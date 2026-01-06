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

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

void main()
{
    // The world vector acts as the normal of the tangent surface
    vec3 N = normalize(v_WorldPos);
    
    vec3 irradiance = vec3(0.0);
    
    // Construct tangent-space basis
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));
    
    // Hemisphere sampling parameters
    float sampleDelta = 0.025;  // Angular step size
    float nrSamples = 0.0;
    
    // Sample hemisphere aligned with N
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // Spherical to Cartesian (tangent space)
            vec3 tangentSample = vec3(
                sin(theta) * cos(phi),
                sin(theta) * sin(phi),
                cos(theta)
            );
            
            // Transform to world space
            vec3 sampleVec = tangentSample.x * right + 
                             tangentSample.y * up + 
                             tangentSample.z * N;
            
            // Sample environment and weight by cos(theta) * sin(theta)
            // cos(theta) = Lambert's law
            // sin(theta) = solid angle correction (more area near equator)
            irradiance += texture(u_EnvironmentMap, sampleVec).rgb * 
                          cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    
    // Normalize: PI factor accounts for hemisphere solid angle
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    FragColor = vec4(irradiance, 1.0);
}
