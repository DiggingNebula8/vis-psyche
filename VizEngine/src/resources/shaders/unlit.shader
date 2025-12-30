#shader vertex
#version 460 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec4 v_Color;
out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * aPos;
	v_Color = aColor;
	v_TexCoord = aTexCoord;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec4 v_Color;
in vec2 v_TexCoord;

uniform vec4 u_ObjectColor;
uniform sampler2D u_MainTex;

void main()
{
	vec4 texColor = texture(u_MainTex, v_TexCoord);
	vec3 combinedColor = v_Color.rgb * u_ObjectColor.rgb * texColor.rgb;
	FragColor = vec4(combinedColor, texColor.a * u_ObjectColor.a);
}


