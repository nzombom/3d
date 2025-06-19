#version 460 core

in vec3 pos;
in vec3 oPos;
in vec3 normal;
in vec3 cameraPos;
in vec4 color;

out vec4 FragColor;

void main() {
	FragColor = vec4(oPos, 1.0);
}
