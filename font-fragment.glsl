#version 330 core

out vec4 frag_color;
in vec4 color;

void main() {
	//frag_color = vec4(1.0, 1.0, 1.0, 0.4);
	//frag_color = vec4(0.0, 0.0, 0.0, 1.0);
	frag_color = color;
}
