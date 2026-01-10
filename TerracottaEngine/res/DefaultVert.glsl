#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in float a_texIndex;

out vec2 v_texCoord;
out float v_texIndex;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	// Later add model matrix after view?
	gl_Position = u_projection * u_view * vec4(a_pos, 1.0);
	v_texCoord = a_texCoord;
	v_texIndex = a_texIndex;
}