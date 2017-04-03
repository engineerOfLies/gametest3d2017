#version 430
layout(location = 0) in vec3 position;
layout(location = 2) in vec3 color;
out vec4 theColor;
uniform sampler2D skin
void main()
{
	gl_Position = vec4(position,1.0);
	theColor = color;
}
