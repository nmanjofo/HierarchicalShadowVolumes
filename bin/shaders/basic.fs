#version 330 core

uniform vec3 color;

out vec4 c;

void main()
{
	if(gl_FrontFacing)
		c = vec4(color, 1);
	else
		c = vec4(1, 1, 0, 1);
}