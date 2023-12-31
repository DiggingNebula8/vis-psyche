#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec2 texCoord;

out vec4 v_VertColor;
out vec2 v_TexCoord;

uniform float u_Scale;

void main()
{
	gl_Position = vec4(aPos.xyz * u_Scale, 1.0);
	v_VertColor = vertColor;
	v_TexCoord = texCoord;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec4 v_VertColor;
in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_MainTex;
void main()
{
	vec4 textureColor = texture(u_MainTex, v_TexCoord);
	vec3 combinedColor = v_VertColor.rgb * u_Color.rgb * textureColor.rgb;
	FragColor = vec4(combinedColor, textureColor.a);
}