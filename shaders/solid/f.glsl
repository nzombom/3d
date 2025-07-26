#version 460 core
in vec3 pos;
in vec3 vNormal;
in vec3 cameraPos;

uniform struct Light {
	vec3 pos;
	vec3 intensity;
} light;

uniform struct Material {
	vec3 color;
	float specular;
} mtl;

out vec4 FragColor;

void main() {
	vec3 N = normalize(vNormal);
	vec3 L = normalize(light.pos - pos);
	vec3 R = 2 * N * dot(L, N) - L;
	vec3 V = normalize(cameraPos - pos);

	float av = 0.125, dv = 0, sv = 0;
	float lxn = dot(L, N);
	if (lxn > 0) dv = lxn;
	float rxv = dot(R, V);
	if (rxv > 0) sv = pow(rxv, 8);

	float d = length(light.pos - pos);
	float atten = 1 / (d * d);
	vec3 final = (av + dv + sv * mtl.specular) * mtl.color
		* light.intensity * atten;

	FragColor = vec4(final, 1.0);
}
