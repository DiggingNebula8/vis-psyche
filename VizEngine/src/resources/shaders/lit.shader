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

uniform mat4 u_Model;
uniform mat4 u_MVP;

void main()
{
	// Fragment position in world space (for lighting calculations)
	v_FragPos = vec3(u_Model * aPos);
	
	// Transform normal to world space
	// Note: For non-uniform scaling, use inverse transpose of model matrix
	v_Normal = mat3(transpose(inverse(u_Model))) * aNormal;
	
	v_Color = aColor;
	v_TexCoord = aTexCoord;
	
	gl_Position = u_MVP * aPos;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 v_Color;
in vec2 v_TexCoord;

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
uniform float u_Shininess;

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
	// Constant base illumination
	vec3 ambient = u_LightAmbient * baseColor;
	
	// === DIFFUSE ===
	// Lambert's cosine law: more light when surface faces the light
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = u_LightDiffuse * diff * baseColor;
	
	// === SPECULAR ===
	// Blinn-Phong specular: uses half vector instead of reflection
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfDir), 0.0), u_Shininess);
	vec3 specular = u_LightSpecular * spec;
	
	// Combine all lighting components
	vec3 result = ambient + diffuse + specular;
	
	FragColor = vec4(result, texColor.a * u_ObjectColor.a);
}

