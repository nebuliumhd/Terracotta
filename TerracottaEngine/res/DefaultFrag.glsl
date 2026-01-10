#version 460 core
layout (location = 0) out vec4 f_color;

in vec2 v_texCoord;
in float v_texIndex;

uniform sampler2D u_textures[32];

void main()
{
	int index = int(v_texIndex);
	f_color = texture(u_textures[index], v_texCoord);
}