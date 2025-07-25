#version 460 core
in vec3 pos;
in vec3 vNormal;
in vec3 cameraPos;

out vec4 FragColor;

void main() {
	vec3 normal = normalize(vNormal);
	FragColor = vec4(normal, 1.0);
}
