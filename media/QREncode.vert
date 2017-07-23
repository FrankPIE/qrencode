#version 330

//////////////////////////////////////
//MVP Matrix
uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

//////////////////////////////////////
//Vertex Attribute
layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec2 uv;

out vec2 TexCoord0;

void main()
{
	gl_Position = projection * view * world * vec4(position, 1.0f);
	TexCoord0 = uv;
}