#version 460 core

in vec3 pos;
in vec3 vNormal;
in vec3 cameraPos;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gColor;

void main() {
	gPosition = vec4(pos, 1.0);
	gNormal = vec4(normalize(vNormal), 1.0);
	gColor = vec4(1.0, 0.5, 0.5, 0.25);
}
