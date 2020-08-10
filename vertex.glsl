#version 330 core

out vec4 color;


uniform mat4 mvp;

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec4 vcolor;


void main()
{
	gl_Position = mvp * vec4(vpos.xyz, 1.0);
	//color = vec4(vcolor.xyz, 1.0);
	color = vec4(vcolor.xyzw);
}
