#version 460 core

in vec3 pos;
in vec3 vNormal;
in vec3 cameraPos;

uniform vec3 lightPos;
uniform vec3 lightStrength;
uniform float lightRadius;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float highlight;

out vec4 FragColor;

void main() {
	const vec3 normal = normalize(vNormal);

	vec3 light = normalize(lightPos - pos);
	float d = length(light);
	vec3 view = normalize(cameraPos - pos);
	vec3 halfway = normalize(light + view);
	float ln = dot(light, normal);
	float hn = dot(halfway, normal);

	float av, dv, sv;
	av = 0.25;
	if (ln > 0) {
		dv = ln;
		if (hn > 0) {
			sv = pow(hn, highlight);
		}
	}

	float s = d / lightRadius;
	float attenuation;
	if (s < 0 || s > 1) attenuation = 0;
	else attenuation = pow(1 - s * s, 2) / (1 + s * s);
	vec3 lv = lightStrength
		* (diffuse * (dv + av) + specular * sv)
		* attenuation;
	FragColor = vec4(lv, 1.0);
}
