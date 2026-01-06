#shader vertex
#version 460 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec4 v_Color;
out vec2 v_TexCoord;
out vec4 v_FragPosLightSpace;  // Position in light space for shadow mapping

uniform mat4 u_Model;
uniform mat4 u_MVP;
uniform mat4 u_LightSpaceMatrix;  // Light's projection * view

void main()
{
	// World position for lighting and shadow calculations
	vec4 worldPos = u_Model * aPos;
	v_FragPos = worldPos.xyz;
	
	// Transform normal to world space
	// Note: For non-uniform scaling, use inverse transpose of model matrix
	v_Normal = mat3(transpose(inverse(u_Model))) * aNormal;
	
	v_Color = aColor;
	v_TexCoord = aTexCoord;
	
	// Transform position to light space for shadow mapping
	v_FragPosLightSpace = u_LightSpaceMatrix * worldPos;
	
	gl_Position = u_MVP * aPos;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 v_Color;
in vec2 v_TexCoord;
in vec4 v_FragPosLightSpace;

// Light properties
uniform vec3 u_LightDirection;
uniform vec3 u_LightAmbient;
uniform vec3 u_LightDiffuse;
uniform vec3 u_LightSpecular;

// Camera position (for specular)
uniform vec3 u_ViewPos;

// Object properties
uniform vec4 u_ObjectColor;
uniform sampler2D u_MainTex;

// Material properties
uniform float u_Roughness;

// Shadow mapping
uniform sampler2D u_ShadowMap;

// Calculate shadow with PCF (Percentage Closer Filtering)
// Returns 0.0 = fully lit, 1.0 = fully in shadow
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	// Perspective divide to get NDC coordinates
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	// Transform from [-1, 1] to [0, 1] range for texture sampling
	projCoords = projCoords * 0.5 + 0.5;
	
	// Outside shadow map bounds = no shadow
	if (projCoords.z > 1.0)
		return 0.0;
	if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
		return 0.0;
	
	float currentDepth = projCoords.z;
	
	// Slope-scaled bias to prevent shadow acne
	// Surfaces facing away from light need more bias
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
	
	// PCF: Sample 3x3 kernel and average for soft shadows
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
	
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			// Sample neighboring texel
			vec2 offset = vec2(x, y) * texelSize;
			float closestDepth = texture(u_ShadowMap, projCoords.xy + offset).r;
			
			// Accumulate shadow comparison
			shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
		}
	}
	
	// Average over 9 samples
	shadow /= 9.0;
	
	return shadow;
}

void main()
{
	// Sample texture
	vec4 texColor = texture(u_MainTex, v_TexCoord);
	
	// Base color: texture * vertex color * object color
	vec3 baseColor = texColor.rgb * v_Color.rgb * u_ObjectColor.rgb;
	
	// Normalize the normal (interpolation can denormalize it)
	vec3 norm = normalize(v_Normal);
	
	// Light direction (pointing FROM light TO fragment, so we negate)
	vec3 lightDir = normalize(-u_LightDirection);
	
	// === AMBIENT ===
	// Constant base illumination (always present, even in shadow)
	vec3 ambient = u_LightAmbient * baseColor;
	
	// === DIFFUSE ===
	// Lambert's cosine law: more light when surface faces the light
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = u_LightDiffuse * diff * baseColor;
	
	// === SPECULAR ===
	// Blinn-Phong specular: uses half vector instead of reflection
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	// Convert roughness (0=shiny, 1=matte) to Blinn-Phong exponent
	float shininess = mix(256.0, 8.0, u_Roughness);
	float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
	vec3 specular = u_LightSpecular * spec;
	
	// === SHADOW ===
	// Calculate shadow factor (0.0 = lit, 1.0 = shadowed)
	float shadow = CalculateShadow(v_FragPosLightSpace, norm, lightDir);
	
	// Apply shadow to diffuse and specular (NOT to ambient)
	// Ambient light reaches shadowed areas (indirect lighting simulation)
	vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);
	
	FragColor = vec4(result, texColor.a * u_ObjectColor.a);
}
